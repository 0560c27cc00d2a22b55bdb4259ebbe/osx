/*
 * "$Id: xml.h,v 1.1.1.1 2004/07/23 06:26:27 jlovell Exp $"
 *
 *   libgimpprint module loader header
 *
 *   Copyright 1997-2002 Michael Sweet (mike@easysw.com),
 *	Robert Krawitz (rlk@alum.mit.edu) and Michael Natterer (mitch@gimp.org)
 *   Copyright 2002-2003 Roger Leigh (rleigh@debian.org)
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2 of the License, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *   for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef GIMP_PRINT_XML_H
#define GIMP_PRINT_XML_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gimp-print/mxml.h>

typedef int (*stp_xml_parse_func)(stp_mxml_node_t *node, const char *file);

extern void
stp_register_xml_parser(const char *name, stp_xml_parse_func parse_func);

extern void
stp_unregister_xml_parser(const char *name);

extern void stp_register_xml_preload(const char *filename);
extern void stp_unregister_xml_preload(const char *filename);

extern int stp_xml_init_defaults(void);
extern int stp_xml_parse_file(const char *file);

extern long stp_xmlstrtol(const char *value);
extern unsigned long stp_xmlstrtoul(const char *value);
extern double stp_xmlstrtod(const char *textval);

extern void stp_xml_init(void);
extern void stp_xml_exit(void);
extern stp_mxml_node_t *stp_xml_get_node(stp_mxml_node_t *xmlroot, ...);
extern stp_mxml_node_t *stp_xmldoc_create_generic(void);
extern void stp_xml_preinit(void);

extern stp_sequence_t *stp_sequence_create_from_xmltree(stp_mxml_node_t *da);
extern stp_mxml_node_t *stp_xmltree_create_from_sequence(const stp_sequence_t *seq);

extern stp_curve_t *stp_curve_create_from_xmltree(stp_mxml_node_t *da);
extern stp_mxml_node_t *stp_xmltree_create_from_curve(const stp_curve_t *curve);

extern stp_array_t *stp_array_create_from_xmltree(stp_mxml_node_t *array);
extern stp_mxml_node_t *stp_xmltree_create_from_array(const stp_array_t *array);
extern void stp_xml_parse_file_named(const char *name);

#endif /* GIMP_PRINT_XML_H */
/*
 * End of "$Id: xml.h,v 1.1.1.1 2004/07/23 06:26:27 jlovell Exp $".
 */
