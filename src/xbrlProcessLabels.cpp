// Copyright (C) 2014-2016 Roberto Bertolusso
//
// This file is part of XBRL.
//
// XBRL is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// XBRL is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with XBRL. If not, see <http://www.gnu.org/licenses/>.

#include <unordered_map>
#include "XBRL.h"


RcppExport SEXP xbrlProcessLabels(SEXP epaDoc) {
  xmlDocPtr doc = (xmlDocPtr) R_ExternalPtrAddr(epaDoc);

  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  xmlXPathObjectPtr label_res = xmlXPathEvalExpression((xmlChar*) "//*[local-name()='label']", context);
  xmlNodeSetPtr label_nodeset = label_res->nodesetval;
  xmlXPathObjectPtr labelArc_res = xmlXPathEvalExpression((xmlChar*) "//*[local-name()='labelArc']", context);
  xmlNodeSetPtr labelArc_nodeset = labelArc_res->nodesetval;
  xmlXPathObjectPtr loc_res = xmlXPathEvalExpression((xmlChar*) "//*[local-name()='loc']", context);
  xmlNodeSetPtr loc_nodeset = loc_res->nodesetval;
  xmlXPathFreeContext(context);

  int label_nodeset_ln = label_nodeset->nodeNr;

  // Build arc lookup: to -> from
  unordered_map<string, string> arc_to_from;
  for (int j = 0; j < labelArc_nodeset->nodeNr; j++) {
    xmlNodePtr arc_node = labelArc_nodeset->nodeTab[j];
    xmlChar *to   = xmlGetProp(arc_node, (xmlChar*) "to");
    xmlChar *from = xmlGetProp(arc_node, (xmlChar*) "from");
    if (to && from)
      arc_to_from[(char*)to] = (char*)from;
    if (to)   xmlFree(to);
    if (from) xmlFree(from);
  }

  // Build loc lookup: label -> node
  unordered_map<string, xmlNodePtr> loc_by_label;
  for (int k = 0; k < loc_nodeset->nodeNr; k++) {
    xmlNodePtr loc_node = loc_nodeset->nodeTab[k];
    xmlChar *lbl = xmlGetProp(loc_node, (xmlChar*) "label");
    if (lbl) {
      loc_by_label[(char*)lbl] = loc_node;
      xmlFree(lbl);
    }
  }

  CharacterVector elementId(label_nodeset_ln);
  CharacterVector lang(label_nodeset_ln);
  CharacterVector labelRole(label_nodeset_ln);
  CharacterVector labelString(label_nodeset_ln);
  CharacterVector href(label_nodeset_ln);

  for(int i = 0; i < label_nodeset_ln; i++) {
    xmlNodePtr label_node = label_nodeset->nodeTab[i];
    xmlChar *label_label = xmlGetProp(label_node, (xmlChar*) "label");

    if (!label_label) {
      elementId[i] = href[i] = lang[i] = labelRole[i] = labelString[i] = NA_STRING;
      continue;
    }

    string label_str = (char*) label_label;
    xmlFree(label_label);

    auto arc_it = arc_to_from.find(label_str);
    if (arc_it == arc_to_from.end()) {
      elementId[i] = href[i] = lang[i] = labelRole[i] = labelString[i] = NA_STRING;
      continue;
    }

    auto loc_it = loc_by_label.find(arc_it->second);
    if (loc_it == loc_by_label.end()) {
      elementId[i] = href[i] = lang[i] = labelRole[i] = labelString[i] = NA_STRING;
      continue;
    }

    xmlNodePtr loc_node = loc_it->second;

    xmlChar *tmp_str;
    if ((tmp_str = xmlGetProp(loc_node, (xmlChar*) "href"))) {
      href[i] = (char *) tmp_str;
      string str = (char *) tmp_str;
      xmlFree(tmp_str);
      size_t found = str.find("#");
      if (found != string::npos) {
        str.replace(0, found+1, "");
        elementId[i] = str;
      }
    } else {
      href[i] = NA_STRING;
      elementId[i] = NA_STRING;
    }
    if ((tmp_str = xmlGetProp(label_node, (xmlChar*) "lang"))) {
      lang[i] = (char *) tmp_str;
      xmlFree(tmp_str);
    } else {
      lang[i] = NA_STRING;
    }
    if ((tmp_str = xmlGetProp(label_node, (xmlChar*) "role"))) {
      labelRole[i] = (char *) tmp_str;
      xmlFree(tmp_str);
    } else {
      labelRole[i] = NA_STRING;
    }
    if ((tmp_str = xmlNodeListGetString(doc, label_node->xmlChildrenNode, 1))) {
      labelString[i] = (char *) tmp_str;
      xmlFree(tmp_str);
    } else {
      labelString[i] = NA_STRING;
    }
  }
  xmlXPathFreeObject(label_res);
  xmlXPathFreeObject(loc_res);
  xmlXPathFreeObject(labelArc_res);

  if (label_nodeset_ln == 0)
    return R_NilValue;

  return DataFrame::create(Named("elementId")=elementId,
			   Named("lang")=lang,
			   Named("labelRole")=labelRole,
			   Named("labelString")=labelString,
			   Named("href")=href);
}
