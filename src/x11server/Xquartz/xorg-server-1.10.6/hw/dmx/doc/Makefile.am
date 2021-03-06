#  Copyright 2005 Red Hat, Inc.
#
#  Permission to use, copy, modify, distribute, and sell this software
#  and its documentation for any purpose is hereby granted without
#  fee, provided that the above copyright notice appear in all copies
#  and that both that copyright notice and this permission notice
#  appear in supporting documentation, and that the name of Red Hat
#  not be used in advertising or publicity pertaining to distribution
#  of the software without specific, written prior permission.  Red
#  Hat makes no representations about the suitability of this software
#  for any purpose.  It is provided "as is" without express or implied
#  warranty.
#
#  RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
#  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
#  NO EVENT SHALL RED HAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
#  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
#  OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
#  NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
#  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

doc_sources = dmx.xml scaled.xml

# Developer's documentation is not installed
if ENABLE_DEVEL_DOCS
include $(top_srcdir)/doc/xml/xmlrules-noinst.in
endif ENABLE_DEVEL_DOCS

DOXYGEN_HEAD=\
	html/annotated.html

DOXYGEN_REST= \
	dmx.txt \
	scaled.txt \
	html/annotated.html \
	html/bc_s.png \
	html/ChkNotMaskEv_8c.html \
	html/ChkNotMaskEv_8h.html \
	html/ChkNotMaskEv_8h_source.html \
	html/classes.html \
	html/closed.png \
	html/dmx-config_8h.html \
	html/dmx-config_8h_source.html \
	html/dmx_8c.html \
	html/dmx_8h.html \
	html/dmx_8h_source.html \
	html/dmx__glxvisuals_8c.html \
	html/dmx__glxvisuals_8h.html \
	html/dmx__glxvisuals_8h_source.html \
	html/dmxarg_8c.html \
	html/dmxarg_8h.html \
	html/dmxarg_8h_source.html \
	html/dmxbackend_8c.html \
	html/dmxbackend_8h.html \
	html/dmxbackend_8h_source.html \
	html/dmxcb_8c.html \
	html/dmxcb_8h.html \
	html/dmxcb_8h_source.html \
	html/dmxclient_8h.html \
	html/dmxclient_8h_source.html \
	html/dmxcmap_8c.html \
	html/dmxcmap_8h.html \
	html/dmxcmap_8h_source.html \
	html/dmxcommon_8c.html \
	html/dmxcommon_8h.html \
	html/dmxcommon_8h_source.html \
	html/dmxcompat_8c.html \
	html/dmxcompat_8h.html \
	html/dmxcompat_8h_source.html \
	html/dmxconfig_8c.html \
	html/dmxconfig_8h.html \
	html/dmxconfig_8h_source.html \
	html/dmxconsole_8c.html \
	html/dmxconsole_8h.html \
	html/dmxconsole_8h_source.html \
	html/dmxcursor_8c.html \
	html/dmxcursor_8h.html \
	html/dmxcursor_8h_source.html \
	html/dmxdetach_8c.html \
	html/dmxdpms_8c.html \
	html/dmxdpms_8h.html \
	html/dmxdpms_8h_source.html \
	html/dmxdummy_8c.html \
	html/dmxdummy_8h.html \
	html/dmxdummy_8h_source.html \
	html/dmxevents_8c.html \
	html/dmxevents_8h.html \
	html/dmxevents_8h_source.html \
	html/dmxextension_8c.html \
	html/dmxextension_8h.html \
	html/dmxextension_8h_source.html \
	html/dmxfont_8c.html \
	html/dmxfont_8h.html \
	html/dmxfont_8h_source.html \
	html/dmxgc_8c.html \
	html/dmxgc_8h.html \
	html/dmxgc_8h_source.html \
	html/dmxgcops_8c.html \
	html/dmxgcops_8h.html \
	html/dmxgcops_8h_source.html \
	html/dmxinit_8c.html \
	html/dmxinit_8h.html \
	html/dmxinit_8h_source.html \
	html/dmxinput_8c.html \
	html/dmxinput_8h.html \
	html/dmxinput_8h_source.html \
	html/dmxinputinit_8c.html \
	html/dmxinputinit_8h.html \
	html/dmxinputinit_8h_source.html \
	html/dmxlog_8c.html \
	html/dmxlog_8h.html \
	html/dmxlog_8h_source.html \
	html/dmxmap_8c.html \
	html/dmxmap_8h.html \
	html/dmxmap_8h_source.html \
	html/dmxmotion_8c.html \
	html/dmxmotion_8h.html \
	html/dmxmotion_8h_source.html \
	html/dmxparse_8c.html \
	html/dmxparse_8h.html \
	html/dmxparse_8h_source.html \
	html/dmxpict_8c.html \
	html/dmxpict_8h.html \
	html/dmxpict_8h_source.html \
	html/dmxpixmap_8c.html \
	html/dmxpixmap_8h.html \
	html/dmxpixmap_8h_source.html \
	html/dmxprint_8c.html \
	html/dmxprint_8h.html \
	html/dmxprint_8h_source.html \
	html/dmxprop_8c.html \
	html/dmxprop_8h.html \
	html/dmxprop_8h_source.html \
	html/dmxscrinit_8c.html \
	html/dmxscrinit_8h.html \
	html/dmxscrinit_8h_source.html \
	html/dmxshadow_8c.html \
	html/dmxshadow_8h.html \
	html/dmxshadow_8h_source.html \
	html/dmxsigio_8c.html \
	html/dmxsigio_8h.html \
	html/dmxsigio_8h_source.html \
	html/dmxstat_8c.html \
	html/dmxstat_8h.html \
	html/dmxstat_8h_source.html \
	html/dmxsync_8c.html \
	html/dmxsync_8h.html \
	html/dmxsync_8h_source.html \
	html/dmxtodmx_8c.html \
	html/dmxvisual_8c.html \
	html/dmxvisual_8h.html \
	html/dmxvisual_8h_source.html \
	html/dmxwindow_8c.html \
	html/dmxwindow_8h.html \
	html/dmxwindow_8h_source.html \
	html/dmxxinput_8c.html \
	html/doxygen.css \
	html/doxygen.png \
	html/files.html \
	html/ftv2blank.png \
	html/ftv2doc.png \
	html/ftv2folderclosed.png \
	html/ftv2folderopen.png \
	html/ftv2lastnode.png \
	html/ftv2link.png \
	html/ftv2mlastnode.png \
	html/ftv2mnode.png \
	html/ftv2node.png \
	html/ftv2plastnode.png \
	html/ftv2pnode.png \
	html/ftv2splitbar.png \
	html/ftv2vertline.png \
	html/functions.html \
	html/functions_0x62.html \
	html/functions_0x63.html \
	html/functions_0x64.html \
	html/functions_0x65.html \
	html/functions_0x66.html \
	html/functions_0x67.html \
	html/functions_0x68.html \
	html/functions_0x69.html \
	html/functions_0x6b.html \
	html/functions_0x6c.html \
	html/functions_0x6d.html \
	html/functions_0x6e.html \
	html/functions_0x6f.html \
	html/functions_0x70.html \
	html/functions_0x72.html \
	html/functions_0x73.html \
	html/functions_0x74.html \
	html/functions_0x75.html \
	html/functions_0x76.html \
	html/functions_0x77.html \
	html/functions_0x78.html \
	html/functions_0x79.html \
	html/functions_eval.html \
	html/functions_vars.html \
	html/functions_vars_0x62.html \
	html/functions_vars_0x63.html \
	html/functions_vars_0x64.html \
	html/functions_vars_0x65.html \
	html/functions_vars_0x66.html \
	html/functions_vars_0x67.html \
	html/functions_vars_0x68.html \
	html/functions_vars_0x69.html \
	html/functions_vars_0x6b.html \
	html/functions_vars_0x6c.html \
	html/functions_vars_0x6d.html \
	html/functions_vars_0x6e.html \
	html/functions_vars_0x6f.html \
	html/functions_vars_0x70.html \
	html/functions_vars_0x72.html \
	html/functions_vars_0x73.html \
	html/functions_vars_0x74.html \
	html/functions_vars_0x75.html \
	html/functions_vars_0x76.html \
	html/functions_vars_0x77.html \
	html/functions_vars_0x78.html \
	html/functions_vars_0x79.html \
	html/globals.html \
	html/globals_0x61.html \
	html/globals_0x62.html \
	html/globals_0x63.html \
	html/globals_0x64.html \
	html/globals_0x65.html \
	html/globals_0x66.html \
	html/globals_0x67.html \
	html/globals_0x69.html \
	html/globals_0x6b.html \
	html/globals_0x6c.html \
	html/globals_0x6d.html \
	html/globals_0x6e.html \
	html/globals_0x6f.html \
	html/globals_0x70.html \
	html/globals_0x71.html \
	html/globals_0x72.html \
	html/globals_0x73.html \
	html/globals_0x74.html \
	html/globals_0x75.html \
	html/globals_0x76.html \
	html/globals_0x78.html \
	html/globals_0x79.html \
	html/globals_defs.html \
	html/globals_defs_0x62.html \
	html/globals_defs_0x63.html \
	html/globals_defs_0x64.html \
	html/globals_defs_0x65.html \
	html/globals_defs_0x66.html \
	html/globals_defs_0x67.html \
	html/globals_defs_0x6c.html \
	html/globals_defs_0x6d.html \
	html/globals_defs_0x6e.html \
	html/globals_defs_0x6f.html \
	html/globals_defs_0x70.html \
	html/globals_defs_0x72.html \
	html/globals_defs_0x73.html \
	html/globals_defs_0x74.html \
	html/globals_defs_0x75.html \
	html/globals_defs_0x76.html \
	html/globals_enum.html \
	html/globals_eval.html \
	html/globals_func.html \
	html/globals_func_0x61.html \
	html/globals_func_0x63.html \
	html/globals_func_0x64.html \
	html/globals_func_0x65.html \
	html/globals_func_0x67.html \
	html/globals_func_0x69.html \
	html/globals_func_0x6b.html \
	html/globals_func_0x6c.html \
	html/globals_func_0x6d.html \
	html/globals_func_0x6e.html \
	html/globals_func_0x6f.html \
	html/globals_func_0x70.html \
	html/globals_func_0x73.html \
	html/globals_func_0x75.html \
	html/globals_func_0x76.html \
	html/globals_func_0x78.html \
	html/globals_func_0x79.html \
	html/globals_type.html \
	html/globals_vars.html \
	html/index.html \
	html/jquery.js \
	html/lnx-keyboard_8c.html \
	html/lnx-keyboard_8h.html \
	html/lnx-keyboard_8h_source.html \
	html/lnx-ms_8c.html \
	html/lnx-ms_8h.html \
	html/lnx-ms_8h_source.html \
	html/lnx-ps2_8c.html \
	html/lnx-ps2_8h.html \
	html/lnx-ps2_8h_source.html \
	html/nav_f.png \
	html/nav_h.png \
	html/navtree.css \
	html/navtree.js \
	html/open.png \
	html/resize.js \
	html/struct__dmxArg.html \
	html/struct__dmxColormapPriv.html \
	html/struct__DMXConfigComment.html \
	html/struct__DMXConfigDisplay.html \
	html/struct__DMXConfigEntry.html \
	html/struct__DMXConfigFullDim.html \
	html/struct__DMXConfigNumber.html \
	html/struct__DMXConfigOption.html \
	html/struct__DMXConfigPair.html \
	html/struct__DMXConfigParam.html \
	html/struct__DMXConfigPartDim.html \
	html/struct__DMXConfigString.html \
	html/struct__DMXConfigSub.html \
	html/struct__DMXConfigToken.html \
	html/struct__DMXConfigVirtual.html \
	html/struct__DMXConfigWall.html \
	html/struct__dmxCursorPriv.html \
	html/struct__DMXEventMap.html \
	html/struct__dmxFontPriv.html \
	html/struct__dmxGCPriv.html \
	html/struct__dmxGlyphPriv.html \
	html/struct__DMXInputInfo.html \
	html/struct__DMXLocalInitInfo.html \
	html/struct__DMXLocalInputInfo.html \
	html/struct__dmxPictPriv.html \
	html/struct__dmxPixPriv.html \
	html/struct__DMXScreenInfo.html \
	html/struct__DMXStatAvg.html \
	html/struct__DMXStatInfo.html \
	html/struct__dmxWinPriv.html \
	html/struct__myPrivate.html \
	html/structDMXConfigCmdStruct.html \
	html/structDMXConfigListStruct.html \
	html/structDMXDesktopAttributesRec.html \
	html/structdmxGlxVisualPrivate.html \
	html/structDMXInputAttributesRec.html \
	html/structDMXScreenAttributesRec.html \
	html/structDMXWindowAttributesRec.html \
	html/structstack.html \
	html/tab_a.png \
	html/tab_b.png \
	html/tab_h.png \
	html/tab_s.png \
	html/tabs.css \
	html/usb-common_8c.html \
	html/usb-common_8h.html \
	html/usb-common_8h_source.html \
	html/usb-keyboard_8c.html \
	html/usb-keyboard_8h.html \
	html/usb-keyboard_8h_source.html \
	html/usb-mouse_8c.html \
	html/usb-mouse_8h.html \
	html/usb-mouse_8h_source.html \
	html/usb-other_8c.html \
	html/usb-other_8h.html \
	html/usb-other_8h_source.html \
	html/usb-private_8h.html \
	html/usb-private_8h_source.html \
	html/vdltodmx_8c.html \
	html/xdmxconfig_8c.html

DOXYGEN_FILES=$(DOXYGEN_HEAD) $(DOXYGEN_REST)

EXTRA_DIST = \
	$(XML_FILES) \
	DMXSpec.txt \
	DMXSpec-v1.txt \
	doxygen.conf \
	doxygen.css \
	doxygen.foot \
	doxygen.head \
	$(DOXYGEN_FILES)

if ENABLE_DEVEL_DOCS
if HAVE_DOXYGEN

DOXYGEN_SRC=doxygen.head doxygen.foot doxygen.css doxygen.conf

all-local: $(DOXYGEN_FILES)

dist-local: $(DOXYGEN_FILES)

$(DOXYGEN_HEAD): $(DOXYGEN_SRC)
	$(DOXYGEN) doxygen.conf

$(DOXYGEN_REST): $(DOXYGEN_HEAD)

maintainer-clean-local:
	rm -rf html/ scaled.txt dmx.txt

distclean-local:
	rm -rf html/ scaled.txt dmx.txt

endif HAVE_DOXYGEN
endif ENABLE_DEVEL_DOCS

$(builddir)/doxygen.head:
	$(LN_S) $(srcdir)/doxygen.head $@

$(builddir)/doxygen.foot:
	$(LN_S) $(srcdir)/doxygen.foot $@

$(builddir)doxygen.css:
	$(LN_S) $(srcdir)/doxygen.css $@

