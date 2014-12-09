/*
 * Copyright (c) 1998-2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All rights reserved.
 *
 *  DRI: Dave Radcliffe
 *
 */
#include <sys/cdefs.h>

__BEGIN_DECLS
#include <ppc/proc_reg.h>
#include <ppc/machine_routines.h>

/* Map memory map IO space */
#include <mach/mach_types.h>
extern vm_offset_t ml_io_map(vm_offset_t phys_addr, vm_size_t size);
__END_DECLS

#include <IOKit/IODeviceTreeSupport.h>
#include <IOKit/IOKitKeys.h>
#include "MacRISC2.h"
#include <IOKit/pci/IOPCIDevice.h>

static unsigned long macRISC2Speed[] = { 0, 1 };

#include <IOKit/pwr_mgt/RootDomain.h>
#include "IOPMSlotsMacRISC2.h"
#include "IOPMUSBMacRISC2.h"
#include <IOKit/pwr_mgt/IOPMPagingPlexus.h>
#include <IOKit/pwr_mgt/IOPMPowerSource.h>
#include <pexpert/pexpert.h>

#include "PB6_1_PlatformMonitor.h"

extern char *gIOMacRISC2PMTree;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define super ApplePlatformExpert

OSDefineMetaClassAndStructors(MacRISC2PE, ApplePlatformExpert);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool MacRISC2PE::start(IOService *provider)
{
    long            		machineType;
    OSData          		*tmpData;
    IORegistryEntry 		*uniNRegEntry;
    IORegistryEntry 		*powerMgtEntry;
    UInt32			   		*primInfo;
	UInt32					stepType, bitsSet, bitsClear;
	bool					result;
	
    setChipSetType(kChipSetTypeCore2001);
	
    // Set the machine type.
    provider_name = provider->getName();  

	machineType = kMacRISC2TypeUnknown;
	doPlatformPowerMonitor = false;
	if (provider_name != NULL) {
		if (0 == strncmp(provider_name, "PowerMac", strlen("PowerMac")))
			machineType = kMacRISC2TypePowerMac;
		else if (0 == strncmp(provider_name, "RackMac", strlen("RackMac")))
			machineType = kMacRISC2TypePowerMac;
		else if (0 == strncmp(provider_name, "PowerBook", strlen("PowerBook")))
			machineType = kMacRISC2TypePowerBook;
		else if (0 == strncmp(provider_name, "iBook", strlen("iBook")))
			machineType = kMacRISC2TypePowerBook;
		else	// kMacRISC2TypeUnknown
			IOLog ("AppleMacRISC2PE - warning: unknown machineType\n");
	}
	
	isPortable = (machineType == kMacRISC2TypePowerBook);
	
    setMachineType(machineType);
	
    // Get the bus speed from the Device Tree.
    tmpData = OSDynamicCast(OSData, provider->getProperty("clock-frequency"));
    if (tmpData == 0) return false;
    macRISC2Speed[0] = *(unsigned long *)tmpData->getBytesNoCopy();
    
    // If this machine is a P99, P84, P72D, Q16, Q41, or Q54, it has a platform monitor, which we'll load later in this function
    hasPMon = (!strcmp (provider_name, "PowerBook5,1")) || (!strcmp (provider_name, "PowerBook5,2")) || (!strcmp (provider_name, "PowerBook5,3"));
   	
	// get uni-N version for use by platformAdjustService
	uniNRegEntry = provider->childFromPath("uni-n", gIODTPlane);
	if (uniNRegEntry == 0) return false;
    tmpData = OSDynamicCast(OSData, uniNRegEntry->getProperty("device-rev"));
    if (tmpData == 0) return false;
    uniNVersion = ((unsigned long *)tmpData->getBytesNoCopy())[0];
   	
    // Get PM features and private features
    powerMgtEntry = retrievePowerMgtEntry ();
    if (powerMgtEntry == 0)
    {
        kprintf ("didn't find power mgt node\n");
        return false;
    }

    tmpData  = OSDynamicCast(OSData, powerMgtEntry->getProperty ("prim-info"));
    if (tmpData != 0)
    {
        primInfo = (unsigned long *)tmpData->getBytesNoCopy();
        if (primInfo != 0)
        {
            _pePMFeatures            = primInfo[3];
            _pePrivPMFeatures        = primInfo[4];
            _peNumBatteriesSupported = ((primInfo[6]>>16) & 0x000000FF);
            kprintf ("Public PM Features: %0x.\n",_pePMFeatures);
            kprintf ("Privat PM Features: %0x.\n",_pePrivPMFeatures);
            kprintf ("Num Internal Batteries Supported: %0x.\n", _peNumBatteriesSupported);
        }
    }
  
    // This is to make sure that  is PMRegisterDevice reentrant
    mutex = IOLockAlloc();
    if (mutex == NULL)
		return false;
    else
		IOLockInit( mutex );
	
    // Set up processorSpeedChangeFlags depending on platform
	processorSpeedChangeFlags = kNoSpeedChange;
	stepType = 0;
    if (machineType == kMacRISC2TypePowerBook) {
		OSIterator 		*childIterator;
		IORegistryEntry *cpuEntry, *powerPCEntry;
		OSData			*cpuSpeedData, *stepTypeData;

		// locate the first PowerPC,xx cpu node so we can get clock properties
		cpuEntry = provider->childFromPath("cpus", gIODTPlane);
		if ((childIterator = cpuEntry->getChildIterator (gIODTPlane)) != NULL) {
			while ((powerPCEntry = (IORegistryEntry *)(childIterator->getNextObject ())) != NULL) {
				if (!strncmp ("PowerPC", powerPCEntry->getName(gIODTPlane), strlen ("PowerPC"))) {
					// Look for dynamic power step feature
					stepTypeData = OSDynamicCast( OSData, powerPCEntry->getProperty( "dynamic-power-step" ));
					if (stepTypeData)
						processorSpeedChangeFlags = kProcessorBasedSpeedChange | kProcessorFast | 
							kL3CacheEnabled | kL2CacheEnabled;
					else {	// Look for forced-reduced-speed case
						stepTypeData = OSDynamicCast( OSData, powerPCEntry->getProperty( "force-reduced-speed" ));
						cpuSpeedData = OSDynamicCast( OSData, powerPCEntry->getProperty( "max-clock-frequency" ));
						if (stepTypeData && cpuSpeedData) {
							// Platform requires environmentally forced speed changes possibly overriding user
							// choices.  These might include slowing down when charging with a weak charger or 
							// reducing speed when the lid is closed to avoid heat buildup.
							UInt32 newCPUSpeed, newNum;
							
							doPlatformPowerMonitor = true;
				
							// At minimum disable L3 cache
							// Note that caches are enabled at this point, but the processor may not be at full speed.
							processorSpeedChangeFlags = kDisableL3SpeedChange | kL3CacheEnabled | kL2CacheEnabled;

							if (stepTypeData->getLength() > 0)
								stepType = *(UInt32 *) stepTypeData->getBytesNoCopy();
				
							newCPUSpeed = *(UInt32 *) cpuSpeedData->getBytesNoCopy();
							if (newCPUSpeed != gPEClockFrequencyInfo.cpu_clock_rate_hz) {
								// If max cpu speed is greater than what OF reported to us
								// then enable PMU speed change in addition to L3 speed change
								// and assuming platform supports that feature.
								if ((_pePrivPMFeatures & (1 << 17)) != 0)
									processorSpeedChangeFlags |= kPMUBasedSpeedChange;
								processorSpeedChangeFlags |= kEnvironmentalSpeedChange;
								// Also fix up internal clock rates
								newNum = newCPUSpeed / (gPEClockFrequencyInfo.cpu_clock_rate_hz /
														gPEClockFrequencyInfo.bus_to_cpu_rate_num);
								gPEClockFrequencyInfo.bus_to_cpu_rate_num = newNum;		// Set new numerator
								gPEClockFrequencyInfo.cpu_clock_rate_hz = newCPUSpeed;	// Set new speed
							}
						} else { // All other notebooks
							// Enable PMU speed change, if platform supports it.  Note that there is also
							// an implicit assumption here that machine started up in fastest mode.
							if ((_pePrivPMFeatures & (1 << 17)) != 0) {
								processorSpeedChangeFlags = kPMUBasedSpeedChange | kProcessorFast | 
									kL3CacheEnabled | kL2CacheEnabled;
								// Some platforms need to disable the L3 at slow speed.  Since we're
								// already assuming the machine started up fast, just set a flag
								// that will cause L3 to be toggled when the speed is changed.
								if ((_pePrivPMFeatures & (1<<21)) != 0)
									processorSpeedChangeFlags |= kDisableL3SpeedChange;
							}
						}
					}
					break;
				}
			}
			childIterator->release();
		}
	}
	
	// Create PlatformFunction nub
	OSDictionary *dict = OSDictionary::withCapacity(2);
	if (dict) {
		const OSSymbol *nameKey, *compatKey, *nameValueSymbol;
		const OSData *nameValueData, *compatValueData;
		char tmpName[32], tmpCompat[128];
		
		nameKey = OSSymbol::withCStringNoCopy("name");
		strcpy(tmpName, "IOPlatformFunction");
		nameValueSymbol = OSSymbol::withCString(tmpName);
		nameValueData = OSData::withBytes(tmpName, strlen(tmpName)+1);
		dict->setObject (nameKey, nameValueData);
		compatKey = OSSymbol::withCStringNoCopy("compatible");
		strcpy (tmpCompat, "IOPlatformFunctionNub");
		compatValueData = OSData::withBytes(tmpCompat, strlen(tmpCompat)+1);
		dict->setObject (compatKey, compatValueData);
		if (plFuncNub = IOPlatformExpert::createNub (dict)) {
			if (!plFuncNub->attach( this ))
				IOLog ("NUB ATTACH FAILED for IOPlatformFunctionNub\n");
			plFuncNub->setName (nameValueSymbol);

			plFuncNub->registerService();
		}
		dict->release();
		nameValueSymbol->release();
		nameKey->release();
		nameValueData->release();
		compatKey->release();
		compatValueData->release();
	}
	
	/*
	 * Call super::start *before* we create specialized child nubs.  Child drivers for these nubs
	 * want to call publishResource and publishResource needs the IOResources node to exist 
	 * if not we'll get a message like "not registry member at registerService()"
	 * super::start() takes care of creating IOResources
	 */
	result = super::start(provider);
	
	// Create PlatformMonitor nub
	if (hasPMon) {
		OSDictionary *dict = OSDictionary::withCapacity(2);
		
		if (dict) {
			const OSSymbol *nameKey, *compatKey, *nameValueSymbol;
			const OSData *nameValueData, *compatValueData;
			char tmpName[32], tmpCompat[128];
			
			nameKey = OSSymbol::withCStringNoCopy("name");
			strcpy(tmpName, "IOPlatformMonitor");
			nameValueSymbol = OSSymbol::withCString(tmpName);
			nameValueData = OSData::withBytes(tmpName, strlen(tmpName)+1);
			dict->setObject (nameKey, nameValueData);
			compatKey = OSSymbol::withCStringNoCopy("compatible");
                        if ((!strcmp(provider_name, "PowerBook5,2")) || 	// Q16
                            (!strcmp(provider_name, "PowerBook5,3"))) { 	// Q41
                            strcpy (tmpCompat, "Portable2003");
                        } else {
                            strcpy (tmpCompat, provider_name);
                        }
			strcat (tmpCompat, "_PlatformMonitor");
			compatValueData = OSData::withBytes(tmpCompat, strlen(tmpCompat)+1);
			dict->setObject (compatKey, compatValueData);
			if (ioPMonNub = IOPlatformExpert::createNub (dict)) {
				if (!ioPMonNub->attach( this ))
					IOLog ("NUB ATTACH FAILED\n");
				ioPMonNub->setName (nameValueSymbol);

				ioPMonNub->registerService();
			}
			dict->release();
			nameValueSymbol->release();
			nameKey->release();
			nameValueData->release();
			compatKey->release();
			compatValueData->release();
		}
    }

    
	// Init power monitor states.  This should be driven by data in the device-tree
	if (doPlatformPowerMonitor) {
		
		bitsSet = kIOPMACInstalled | kIOPMACnoChargeCapability;
		bitsClear = 0;
		powerMonWeakCharger.bitsXor = bitsSet & ~bitsClear;
		powerMonWeakCharger.bitsMask = bitsSet | bitsClear;
		
		bitsSet = kIOPMRawLowBattery;
		bitsClear = 0;
		powerMonBatteryWarning.bitsXor = bitsSet & ~bitsClear;
		powerMonBatteryWarning.bitsMask = bitsSet | bitsClear;
		
		bitsSet = kIOPMBatteryDepleted;
		bitsClear = 0;
		powerMonBatteryDepleted.bitsXor = bitsSet & ~bitsClear;
		powerMonBatteryDepleted.bitsMask = bitsSet | bitsClear;
		
		bitsSet = 0;
		bitsClear = kIOPMBatteryInstalled;
		powerMonBatteryNotInstalled.bitsXor = bitsSet & ~bitsClear;
		powerMonBatteryNotInstalled.bitsMask = bitsSet | bitsClear;
		
		if ((stepType & 1) == 0) {
			bitsSet = kIOPMClosedClamshell;
			bitsClear = 0;
			powerMonClamshellClosed.bitsXor = bitsSet & ~bitsClear;
			powerMonClamshellClosed.bitsMask = bitsSet | bitsClear;
		} else {	// Don't do anything on clamshell closed
			powerMonClamshellClosed.bitsXor = 0;
			powerMonClamshellClosed.bitsMask = ~0L;
		}
	} else { // Assume no power monitoring
		powerMonWeakCharger.bitsXor = 0;
		powerMonWeakCharger.bitsMask = ~0L;
		powerMonBatteryWarning.bitsXor = 0;
		powerMonBatteryWarning.bitsMask = ~0L;
		powerMonBatteryDepleted.bitsXor = 0;
		powerMonBatteryDepleted.bitsMask = ~0L;
		powerMonBatteryNotInstalled.bitsXor = 0;
		powerMonBatteryNotInstalled.bitsMask = ~0L;
		powerMonClamshellClosed.bitsXor = 0;
		powerMonClamshellClosed.bitsMask = ~0L;
	}

    return result;
}

IORegistryEntry * MacRISC2PE::retrievePowerMgtEntry (void)
{
    IORegistryEntry *     theEntry = 0;
    IORegistryEntry *     anObj = 0;
    IORegistryIterator *  iter;
    OSString *            powerMgtNodeName;

    iter = IORegistryIterator::iterateOver (IORegistryEntry::getPlane(kIODeviceTreePlane), kIORegistryIterateRecursively);
    if (iter)
    {
        powerMgtNodeName = OSString::withCString("power-mgt");
        anObj = iter->getNextObject ();
        while (anObj)
        {
            if (anObj->compareName(powerMgtNodeName))
            {
                theEntry = anObj;
                break;
            }
            anObj = iter->getNextObject();
        }
    
        powerMgtNodeName->release();
        iter->release ();
    }

    return theEntry;
}

bool MacRISC2PE::platformAdjustService(IOService *service)
{
    bool           result;
  
	/*
		
	this is for 3290321 & 3383856 to patch up audio components of an improper device tree. 
	the following properties need to be removed from any platform that has the
	"has-anded-reset" property it's sound node.
	
	extint-gpio4's	'audio-gpio' property with value of 'line-input-detect'
					'audio-gpio-active-state' property

	gpio5's			'audio-gpio' property with value of 'headphone-mute'
					'audio-gpio-active-state' property

	gpio6's			'audio-gpio' property with value of 'amp-mute'
					'audio-gpio-active-state' property

	gpio11's		'audio-gpio' property with value of 'audio-hw-reset'
					'audio-gpio-active-state' property

	extint-gpio15's	'audio-gpio' property with value of 'headphone-detect'
					'audio-gpio-active-state' property
			
	*/
	
    if(!strcmp(service->getName(), "sound"))
	{
		OSObject			*hasAndedReset;
		const OSSymbol		*audioSymbol = OSSymbol::withCString("audio-gpio");
		const OSSymbol		*activeStateSymbol = OSSymbol::withCString("audio-gpio-active-state");

		IORegistryEntry		*gpioNode, *childNode;
		OSIterator			*childIterator;
		OSData				*tmpOSData;

		hasAndedReset = service->getProperty("has-anded-reset", gIODTPlane);
		
		if(hasAndedReset)
		{
			gpioNode = IORegistryEntry::fromPath("mac-io/gpio", gIODTPlane);
			if(gpioNode)
			{
				childIterator = gpioNode->getChildIterator(gIODTPlane);
				if(childIterator)
				{
					IOLog("**** PAS: got childIterator\n");
					
					while((childNode = (IORegistryEntry *)(childIterator->getNextObject())) != NULL)
					{
						if(!strcmp(childNode->getName(), "extint-gpio4"))
						{
							tmpOSData = OSDynamicCast(OSData, childNode->getProperty(audioSymbol));
							if(tmpOSData)
							{
								if(!strcmp((const char *)tmpOSData->getBytesNoCopy(), "line-input-detect"))
								{
									IOLog("**** PAS: deleting %s with value %s\n", (const char *)audioSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
									childNode->removeProperty(audioSymbol);

									tmpOSData = OSDynamicCast(OSData, childNode->getProperty(activeStateSymbol));
									if(tmpOSData)
									{
										IOLog("**** PAS: deleting %s with value %s\n", (const char *)activeStateSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
										
										// we don't care what the returned value is, we just need to delete the property
										childNode->removeProperty(activeStateSymbol);
									}
								}
							}
						} 
						
						else if(!strcmp(childNode->getName(), "extint-gpio15"))
						{
							tmpOSData = OSDynamicCast(OSData, childNode->getProperty(audioSymbol));
							if(tmpOSData)
							{
								if(!strcmp((const char *)tmpOSData->getBytesNoCopy(), "headphone-detect"))
								{
									IOLog("**** PAS: deleting %s with value %s\n", (const char *)audioSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
									childNode->removeProperty(audioSymbol);

									tmpOSData = OSDynamicCast(OSData, childNode->getProperty(activeStateSymbol));
									if(tmpOSData)
									{
										IOLog("**** PAS: deleting %s with value %s\n", (const char *)activeStateSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
										
										// we don't care what the returned value is, we just need to delete the property
										childNode->removeProperty(activeStateSymbol);
									}
								}
							}
						}
					
						else if(!strcmp(childNode->getName(), "gpio5"))
						{
							tmpOSData = OSDynamicCast(OSData, childNode->getProperty(audioSymbol));
							if(tmpOSData)
							{
								if(!strcmp((const char *)tmpOSData->getBytesNoCopy(), "headphone-mute"))
								{
									IOLog("**** PAS: deleting %s with value %s\n", (const char *)audioSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
									childNode->removeProperty(audioSymbol);

									tmpOSData = OSDynamicCast(OSData, childNode->getProperty(activeStateSymbol));
									if(tmpOSData)
									{
										IOLog("**** PAS: deleting %s with value %s\n", (const char *)activeStateSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
										
										// we don't care what the returned value is, we just need to delete the property
										childNode->removeProperty(activeStateSymbol);
									}
								}
							}
						} 
						
						else if(!strcmp(childNode->getName(), "gpio6"))
						{
							tmpOSData = OSDynamicCast(OSData, childNode->getProperty(audioSymbol));
							if(tmpOSData)
							{
								if(!strcmp((const char *)tmpOSData->getBytesNoCopy(), "amp-mute"))
								{
									IOLog("**** PAS: deleting %s with value %s\n", (const char *)audioSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
									childNode->removeProperty(audioSymbol);

									tmpOSData = OSDynamicCast(OSData, childNode->getProperty(activeStateSymbol));
									if(tmpOSData)
									{
										IOLog("**** PAS: deleting %s with value %s\n", (const char *)activeStateSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
										
										// we don't care what the returned value is, we just need to delete the property
										childNode->removeProperty(activeStateSymbol);
									}
								}
							}
						} 
						
						else if(!strcmp(childNode->getName(), "gpio11"))
						{
							tmpOSData = OSDynamicCast(OSData, childNode->getProperty(audioSymbol));
							if(tmpOSData)
							{
								if(!strcmp((const char *)tmpOSData->getBytesNoCopy(), "audio-hw-reset"))
								{
									IOLog("**** PAS: deleting %s with value %s\n", (const char *)audioSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
									childNode->removeProperty(audioSymbol);

									tmpOSData = OSDynamicCast(OSData, childNode->getProperty(activeStateSymbol));
									if(tmpOSData)
									{
										IOLog("**** PAS: deleting %s with value %s\n", (const char *)activeStateSymbol->getCStringNoCopy(), (const char *)tmpOSData->getBytesNoCopy());
										
										// we don't care what the returned value is, we just need to delete the property
										childNode->removeProperty(activeStateSymbol);
									}
								}
							}
						}
					}

                    childIterator->release();
				
				} else
				{
					IOLog("MacRISC2PE::platformAdjustService ERROR - could not find childIterator\n");
					return false;
				}
			
			} else
			{
				IOLog("MacRISC2PE::platformAdjustService ERROR - could not find gpioNode\n");
				return false;
			}
		}
 
		audioSymbol->release();
		activeStateSymbol->release();
		return true;
	}

    if (IODTMatchNubWithKeys(service, "open-pic"))
    {
	const OSSymbol	* keySymbol;
	OSSymbol 	* tmpSymbol;

        keySymbol = OSSymbol::withCStringNoCopy("InterruptControllerName");
        tmpSymbol = (OSSymbol *)IODTInterruptControllerName(service);
        result    = service->setProperty(keySymbol, tmpSymbol);
        return true;
    }

    if (!strcmp(service->getName(), "programmer-switch"))
    {
        // Set property to tell AppleNMI to mask/unmask NMI @ sleep/wake
        service->setProperty("mask_NMI", service); 
        return true;
    }
  
    if (!strcmp(service->getName(), "pmu"))
    {
        // Change the interrupt mapping for pmu source 4.
        OSArray              *tmpArray;
        OSCollectionIterator *extIntList;
        IORegistryEntry      *extInt;
        OSObject             *extIntControllerName;
        OSObject             *extIntControllerData;
    
        // Set the no-nvram property.
        service->setProperty("no-nvram", service);
    
        // Find the new interrupt information.
        extIntList = IODTFindMatchingEntries(getProvider(), kIODTRecursive, "'extint-gpio1'");
        extInt = (IORegistryEntry *)extIntList->getNextObject();
    
        tmpArray = (OSArray *)extInt->getProperty(gIOInterruptControllersKey);
        extIntControllerName = tmpArray->getObject(0);
        tmpArray = (OSArray *)extInt->getProperty(gIOInterruptSpecifiersKey);
        extIntControllerData = tmpArray->getObject(0);
    
        // Replace the interrupt infomation for pmu source 4.
        tmpArray = (OSArray *)service->getProperty(gIOInterruptControllersKey);
        tmpArray->replaceObject(4, extIntControllerName);
        tmpArray = (OSArray *)service->getProperty(gIOInterruptSpecifiersKey);
        tmpArray->replaceObject(4, extIntControllerData);
    
        extIntList->release();
        
        return true;
    }

    if (!strcmp(service->getName(), "via-pmu"))
    {
        service->setProperty("BusSpeedCorrect", this);
        return true;
    }
    
	/*
	 * For uni-n pci version 1.5 which services mac-io, tell pci driver to disable read data gating
	 * -- Note that previous versions of this driver defined version 1.5 as 0x0010.  This is not 
	 * correct and we now use 0x0011 for uni-n 1.5.
	 */
	if ((uniNVersion == kUniNVersion150) && IODTMatchNubWithKeys(service, "('pci', 'uni-north')") && 
		(service->childFromPath("mac-io", gIODTPlane) != NULL)) {
			service->setProperty ("DisableRDG", true);
			return true;
	}
    
	/* 
	 * For Firewire on uni-n Pangea & uni-n 1.5 through 2.0, adjust the cache line size and timer latency
	 */
	if (((uniNVersion >= kUniNVersion150) && (uniNVersion <= kUniNVersion200) || (uniNVersion == kUniNVersionPangea)) &&
		(!strcmp(service->getName(gIODTPlane), "firewire")) &&
		IODTMatchNubWithKeys(service->getParentEntry(gIODTPlane), "('pci', 'uni-north')")) {
			char data;
			
			data = 0x08;		// 32 byte cache line
			service->setProperty (kIOPCICacheLineSize, &data, 1);
			data = 0x40;		// latency timer to 40
			service->setProperty (kIOPCITimerLatency, &data, 1);
			return true;
	}
    
	/* 
	 * For Ethernet on uni-n 2.0, adjust the cache line size and timer latency
	 */
	if ((uniNVersion == kUniNVersion200) &&
		IODTMatchNubWithKeys(service, "('ethernet', 'gmac')") &&
		IODTMatchNubWithKeys(service->getParentEntry(gIODTPlane), "('pci', 'uni-north')")) {
			char 	data;
			long	cacheSize;
			OSData 	*cacheData;
			
			data = 0x08; 	// assume default of 32 byte cache line
			cacheData = OSDynamicCast( OSData, service->getProperty( "cache-line-size" ) );
			if (cacheData) {
				cacheSize = *(long *)cacheData->getBytesNoCopy();
				data = (cacheSize >> 2);
			}
			service->setProperty (kIOPCICacheLineSize, &data, 1);
			data = 0x20;		// latency timer to 20
			service->setProperty (kIOPCITimerLatency, &data, 1);
			return true;
	}
	
	// For usb@18 on PowerBook4,x machines add an "AAPL,SuspendablePorts" property if it doesn't exist
	if (!strcmp(service->getName(), "usb") && 
		(0 == strncmp(provider_name, "PowerBook4,", strlen("PowerBook4,"))) &&
		IODTMatchNubWithKeys(service->getParentEntry(gIODTPlane), "('pci', 'uni-north')")) {
		
		OSData				*regProp;
		IOPCIAddressSpace	*pciAddress;
		UInt32				ports;
	
		if( (regProp = (OSData *) service->getProperty("reg"))) {
			pciAddress = (IOPCIAddressSpace *) regProp->getBytesNoCopy();
			// Only for usb@18
			if (pciAddress->s.deviceNum == 0x18) {
				// If property doesn't exist, create it
				if(!((OSData *) service->getProperty(kAAPLSuspendablePorts))) {
					ports = 4;
					service->setProperty (kAAPLSuspendablePorts, &ports, sizeof(UInt32));
				}
				return true;
			}
		}
	}

	// for P69s (Xserves) with early BootROMs, the 'temp-monitor' node has an incorrect "reg" property
	// -- change the "reg" property to be correct.  ALL P69s, regardless of BootROM, have the
	//    'temp-monitor' I2C device at I2C address 0x92.  It is incorrectly set to 0x192 in early
	//    BootROMs, and causes the AppleCPUThermo driver to attempt to access an invalid device
	//    address.  The failure of that access causes hwmond to not return temperature information
	//    about the CPU to Server Monitor.
	if ( ( strcmp( "temp-monitor", service->getName() ) == 0 ) &&
	     ( strcmp( "RackMac1,1"  , provider_name      ) == 0 ) )    // ALL P69s, regardless of BootROM
	{
	OSData  * deviceType;
	UInt32  newRegPropertyValue;

		deviceType = OSDynamicCast( OSData, service->getProperty( "device_type" ) );
		if ( deviceType && ( strcmp( "ds1775", (char *)deviceType->getBytesNoCopy() ) == 0 ) )
		{
			// if the service is 'temp-monitor' and we are on a P69, and we have verified
			// that the 'temp-monitor' device is of type 'ds1775', go ahead and adjust the
			// "reg" property to be 0x00000092 (bus 0, device address 0x92).
			newRegPropertyValue = 0x92;
			service->setProperty( "reg", &newRegPropertyValue, sizeof( UInt32 ) );

			// return true;             // handled by the default 'return true' at the bottom of the routine
		}
	}

	return true;
}

IOReturn MacRISC2PE::callPlatformFunction(const OSSymbol *functionName,
					bool waitForFunction,
					void *param1, void *param2,
					void *param3, void *param4)
{
    if (functionName == gGetDefaultBusSpeedsKey)
    {
        getDefaultBusSpeeds((long *)param1, (unsigned long **)param2);
        return kIOReturnSuccess;
    }
  
    if (functionName->isEqualTo("EnableUniNEthernetClock"))
    {
        enableUniNEthernetClock((bool)param1, (IOService *)param2);
        return kIOReturnSuccess;
    }

    if (functionName->isEqualTo("EnableFireWireClock")) {
        enableUniNFireWireClock((bool)param1, (IOService *)param2);
        return kIOReturnSuccess;
    }
  
    if (functionName->isEqualTo("EnableFireWireCablePower")) {
        enableUniNFireWireCablePower((bool)param1);
        return kIOReturnSuccess;
    }
  
    if (functionName->isEqualTo("AccessUniN15PerformanceRegister"))
    {
        return accessUniN15PerformanceRegister((bool)param1, (long)param2, (unsigned long *)param3);
    }
  
    if (functionName->isEqualTo("PlatformIsPortable")) {
		*(bool *) param1 = isPortable;
        return kIOReturnSuccess;
    }
  
    if (functionName->isEqualTo("PlatformPowerMonitor")) {
		return platformPowerMonitor ((UInt32 *) param1);
    }
	
    if (functionName->isEqualTo("PerformPMUSpeedChange")) {
		if (!macRISC2CPU)
			macRISC2CPU = OSDynamicCast (MacRISC2CPU, waitForService (serviceMatching("MacRISC2CPU")));
		
		if (macRISC2CPU) {
			macRISC2CPU->performPMUSpeedChange ((UInt32) param1);
			return kIOReturnSuccess;
		}
		
		return kIOReturnUnsupported;
    }
  
    return super::callPlatformFunction(functionName, waitForFunction, param1, param2, param3, param4);
}

void MacRISC2PE::getDefaultBusSpeeds(long *numSpeeds, unsigned long **speedList)
{
    if ((numSpeeds == 0) || (speedList == 0)) return;
  
    *numSpeeds = 1;
    *speedList = macRISC2Speed;
}

void MacRISC2PE::enableUniNEthernetClock(bool enable, IOService *nub)
{
	if (!uniN) 
		uniN = OSDynamicCast (AppleUniN, waitForService(serviceMatching("AppleUniN")));

	if (uniN)
		uniN->enableUniNEthernetClock (enable, nub);
	
	return;
}

void MacRISC2PE::enableUniNFireWireClock(bool enable, IOService *nub)
{
	if (!uniN) 
		uniN = OSDynamicCast (AppleUniN, waitForService(serviceMatching("AppleUniN")));

	if (uniN)
		uniN->enableUniNFireWireClock (enable, nub);
	
	return;
}

void MacRISC2PE::enableUniNFireWireCablePower(bool enable)
{
    // Turn off cable power supply on mid/merc/pismo(on pismo only, this kills the phy)

	const OSSymbol *tmpSymbol = OSSymbol::withCString("keyLargo_writeRegUInt8");

    if(getMachineType() == kMacRISC2TypePowerBook)
    {
        IOService *keyLargo;
        keyLargo = waitForService(serviceMatching("KeyLargo"));
        
        if(keyLargo)
        {
            UInt32 gpioOffset = 0x73;
            
            keyLargo->callPlatformFunction(tmpSymbol, true, (void *)&gpioOffset, (void *)(enable ? 0:4), 0, 0);
        }
    }
	
	tmpSymbol->release();
}

IOReturn MacRISC2PE::accessUniN15PerformanceRegister(bool write, long regNumber, unsigned long *data)
{
	if (!uniN) 
		uniN = OSDynamicCast (AppleUniN, waitForService(serviceMatching("AppleUniN")));

	if (uniN)
		return uniN->accessUniN15PerformanceRegister (write, regNumber, data);
	
	return kIOReturnUnsupported;
}


//*********************************************************************************
// platformPowerMonitor
//
// A call platform function call called by the ApplePMU driver.  ApplePMU call us
// with a set of power flags.  We examine those flags and modify the state
// according to the characteristics of the platform. 
//
// If necessary, we force an immediate change in the power state
//*********************************************************************************
IOReturn MacRISC2PE::platformPowerMonitor(UInt32 *powerFlags)
{
	IOReturn	result;

	/*
	 * If we created an ioPMonNub for this platform, then use it
	 */
	if (ioPMonNub) {
		static UInt32 			i = 0;
		static bool 			pmonInited = false;
		static OSDictionary 	*dict;
		static OSNumber			*powerBits;

		// the first time we're called is on the ApplePMU start thread and we don't
		// want to block it, so just skip the first few events
		if (i < 10) {
			i++;
			return kIOReturnSuccess;
		}

		if ((i == 10) && !ioPMon) {
			IOService *serv;
			
			i = 11;
			serv = waitForService(resourceMatching("IOPlatformMonitor"));
			ioPMon = OSDynamicCast (IOPlatformMonitor, serv->getProperty("IOPlatformMonitor"));
		}

		if (ioPMon) {	// If there's an ioPMon, use it
			if (!pmonInited) {
				dict = OSDictionary::withCapacity(2);
				if (!dict) {
					ioPMon = NULL;
				} else {
				
					powerBits = OSNumber::withNumber ((long long)*powerFlags, 32);
					
					dict->setObject (kIOPMonTypeKey, OSSymbol::withCString(kIOPMonTypeClamshellSens));
					dict->setObject (kIOPMonCurrentValueKey, powerBits);
		
					if (messageClient (kIOPMonMessageRegister, ioPMon, (void *)dict) != kIOReturnSuccess) {
						// IOPMon doesn't need to know about us, so don't bother with it
						IOLog ("MacRISC2PE::platformPowerMonitor - failed to register clamshell with IOPlatformMonitor\n");
						dict->release();
						ioPMon = NULL;
						return kIOReturnUnsupported;
					} 
					pmonInited = true;
					return kIOReturnSuccess;
				}
			}
			
			// save the current value into the dictionary
			powerBits->setValue((long long)*powerFlags);
			messageClient (kIOPMonMessagePowerMonitor, ioPMon, (void *)dict);
			// retrieve the value updated by the IOPMon and return to PMU
			*powerFlags = powerBits->unsigned32BitValue();
		}
		i++;
	
		return kIOReturnSuccess;
	}
	
	if (doPlatformPowerMonitor) {
		// First check primary power conditions
		if ((((*powerFlags ^ powerMonWeakCharger.bitsXor) & powerMonWeakCharger.bitsMask) == 0) ||
			(((*powerFlags ^ powerMonBatteryWarning.bitsXor) & powerMonBatteryWarning.bitsMask) == 0) ||
			(((*powerFlags ^ powerMonBatteryDepleted.bitsXor) & powerMonBatteryDepleted.bitsMask) == 0) ||
			(((*powerFlags ^ powerMonBatteryNotInstalled.bitsXor) & powerMonBatteryNotInstalled.bitsMask) == 0)) {
					/*
					 * For these primary power conditions we signal the power manager to force low power state
					 * This includes both reduced processor speed and disabled L3 cache.
					 */
					*powerFlags |= kIOPMForceLowSpeed;
					/*
					 * If we previously speed changed due to a closed clamshell and the L3 cache is still enabled
					 * we must call through to get the L3 cache disabled as well
					 */
					if (processorSpeedChangeFlags & kL3CacheEnabled) {
						if (!macRISC2CPU)
							macRISC2CPU = OSDynamicCast (MacRISC2CPU, waitForService (serviceMatching("MacRISC2CPU")));
						if (macRISC2CPU) {
							processorSpeedChangeFlags &= ~kClamshellClosedSpeedChange;
							macRISC2CPU->setAggressiveness (kPMSetProcessorSpeed, 1); // Force slow now so cache state is right
						}
					}
		} else if (((*powerFlags ^ powerMonClamshellClosed.bitsXor) & powerMonClamshellClosed.bitsMask) == 0) {
				/*
				 * clamShell closed with no other power conditions is a special case --
				 * leave L3 cache enabled
				 */
				*powerFlags |= kIOPMForceLowSpeed;
				
				if (!(processorSpeedChangeFlags & kL3CacheEnabled)) {
					if (!macRISC2CPU)
						macRISC2CPU = OSDynamicCast (MacRISC2CPU, waitForService (serviceMatching("MacRISC2CPU")));
					if (macRISC2CPU) {
						if (processorSpeedChangeFlags & kPMUBasedSpeedChange) {
							// Only want setAggressiveness to enable cache
							processorSpeedChangeFlags &= ~kPMUBasedSpeedChange;
							macRISC2CPU->setAggressiveness (kPMSetProcessorSpeed, 0); // Force fast now so cache state is right
							processorSpeedChangeFlags |= kPMUBasedSpeedChange;
						}
					}
				}
				processorSpeedChangeFlags |= kClamshellClosedSpeedChange;	// Show clamshell state
		} else {
			/*
			 * No low power conditions exist, clear all flags
			 */
			*powerFlags &= ~kIOPMForceLowSpeed;
			processorSpeedChangeFlags &= ~kClamshellClosedSpeedChange;
		}

		result = kIOReturnSuccess;
	} else
		result = kIOReturnUnsupported;		// Not supported on this platform
		
    return result;
}

//*********************************************************************************
// PMInstantiatePowerDomains
//
// This overrides the vanilla implementation in IOPlatformExpert.  It instantiates
// a root domain with two children, one for the USB bus (to handle the USB idle
// power budget), and one for the expansions slots on the PCI bus (to handle
// the idle PCI power budget)
//*********************************************************************************

void MacRISC2PE::PMInstantiatePowerDomains ( void )
{    
	const OSSymbol *desc = OSSymbol::withCString("powertreedesc");
    IOPMUSBMacRISC2 * usbMacRISC2;

	// Move our power tree description from our driver (where it's a property in the driver)
	// to our provider
	kprintf ("MacRISC2PE::PMInstantiatePowerDomains - getting pmtree property\n");
    thePowerTree = OSDynamicCast(OSArray, getProperty(desc));

    if( 0 == thePowerTree)
    {
        kprintf ("error retrieving power tree\n");
		return;
    }
	kprintf ("MacRISC2PE::PMInstantiatePowerDomains - got pmtree property\n");

    getProvider()->setProperty (desc, thePowerTree);
	
	// No need to keep original around
	removeProperty(desc);
    		
    root = IOPMrootDomain::construct();
    root->attach(this);
    root->start(this);

    if ( plexus ) {
        root->addPowerChild(plexus);
    }

    root->setSleepSupported(kRootDomainSleepSupported);
   
    if (NULL == root)
    {
        kprintf ("PMInstantiatePowerDomains - null ROOT\n");
        return;
    }

    PMRegisterDevice (NULL, root);

    usbMacRISC2 = new IOPMUSBMacRISC2;
    if (usbMacRISC2)
    {
        usbMacRISC2->init ();
        usbMacRISC2->attach (this);
        usbMacRISC2->start (this);
        PMRegisterDevice (root, usbMacRISC2);
        if ( plexus ) {
            plexus->addPowerChild (usbMacRISC2);
        }
    }

    slotsMacRISC2 = new IOPMSlotsMacRISC2;
    if (slotsMacRISC2)
    {
        slotsMacRISC2->init ();
        slotsMacRISC2->attach (this);
        slotsMacRISC2->start (this);
        PMRegisterDevice (root, slotsMacRISC2);
        if ( plexus ) {
            plexus->addPowerChild (slotsMacRISC2);
        }
    }

    if (processorSpeedChangeFlags != kNoSpeedChange) {
        // Any system that support Speed change supports Reduce Processor Speed.
        root->publishFeature("Reduce Processor Speed");
        
        // Enable Dynamic Power Step for low latency systems.
        if (processorSpeedChangeFlags & kProcessorBasedSpeedChange) {
            root->publishFeature("Dynamic Power Step");
        }
    }
    
    return;
}


//*********************************************************************************
// PMRegisterDevice
//
// This overrides the vanilla implementation in IOPlatformExpert.  We try to 
// put a device into the right position within the power domain hierarchy.
//*********************************************************************************
extern const IORegistryPlane * gIOPowerPlane;

void MacRISC2PE::PMRegisterDevice(IOService * theNub, IOService * theDevice)
{
    bool            nodeFound  = false;
    IOReturn        err        = -1;
    OSData *	    propertyPtr = 0;
    const char *    theProperty;

    // Starts the protected area, we are trying to protect numInstancesRegistered
    if (mutex != NULL)
      IOLockLock(mutex);
     
    // reset our tracking variables before we check the XML-derived tree
    multipleParentKeyValue = NULL;
    numInstancesRegistered = 0;

    // try to find a home for this registrant in our XML-derived tree
    nodeFound = CheckSubTree (thePowerTree, theNub, theDevice, NULL);

    if (0 == numInstancesRegistered)
    {
        // make sure the provider is within the Power Plane...if not, 
        // back up the hierarchy until we find a grandfather or great
        // grandfather, etc., that is in the Power Plane.

        while( theNub && (!theNub->inPlane(gIOPowerPlane)))
            theNub = theNub->getProvider();
    }

    // Ends the protected area, we are trying to protect numInstancesRegistered
    if (mutex != NULL)
       IOLockUnlock(mutex);
     
    // try to register with the given (or reassigned in the case above) provider.
    if ( NULL != theNub )
        err = theNub->addPowerChild (theDevice);

    // failing that then register with root (but only if we didn't register in the 
    // XML-derived tree and only if the device we're registering is not the root).
    if ((err != IOPMNoErr) && (0 == numInstancesRegistered) && (theDevice != root)) {
        root->addPowerChild (theDevice);
        if ( plexus ) {
            plexus->addPowerChild (theDevice);
        }
    }

    // in addition, if it's in a PCI slot, give it to the Aux Power Supply driver
    
    propertyPtr = OSDynamicCast(OSData,theDevice->getProperty("AAPL,slot-name"));
    if ( propertyPtr ) {
	theProperty = (const char *) propertyPtr->getBytesNoCopy();
        if ( strncmp("SLOT-",theProperty,5) == 0 ) {
            slotsMacRISC2->addPowerChild (theDevice);
	}
    }
}

