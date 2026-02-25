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

#include "XBRL.h"
#include <vector>

// Extract explicitMember and typedMember children from a container node
// (either <segment> or <scenario>) into the dimension output vectors.
static void extract_dims(xmlDocPtr doc,
                         xmlNodePtr container,
                         const string& ctx_id,
                         vector<string>& dim_ctx,
                         vector<string>& dim_name,
                         vector<string>& dim_value) {
  xmlNodePtr member = container->xmlChildrenNode;
  while (member) {
    if (!xmlStrcmp(member->name, (xmlChar*) "explicitMember")) {
      xmlChar* dim = xmlGetProp(member, (xmlChar*) "dimension");
      xmlChar* val = xmlNodeListGetString(doc, member->xmlChildrenNode, 1);
      dim_ctx.push_back(ctx_id);
      dim_name.push_back(dim ? string((char*)dim) : "");
      dim_value.push_back(val ? string((char*)val) : "");
      if (dim) xmlFree(dim);
      if (val) xmlFree(val);
    } else if (!xmlStrcmp(member->name, (xmlChar*) "typedMember")) {
      xmlChar* dim = xmlGetProp(member, (xmlChar*) "dimension");
      // value is the text content of the first child element
      string val_str;
      xmlNodePtr child = member->xmlChildrenNode;
      while (child) {
        if (child->type == XML_ELEMENT_NODE) {
          xmlChar* val = xmlNodeListGetString(doc, child->xmlChildrenNode, 1);
          if (val) {
            val_str = string((char*)val);
            xmlFree(val);
          }
          break;
        }
        child = child->next;
      }
      dim_ctx.push_back(ctx_id);
      dim_name.push_back(dim ? string((char*)dim) : "");
      dim_value.push_back(val_str);
      if (dim) xmlFree(dim);
    }
    member = member->next;
  }
}

RcppExport SEXP xbrlProcessContexts(SEXP epaDoc) {
  xmlDocPtr doc = (xmlDocPtr) R_ExternalPtrAddr(epaDoc);

  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  xmlXPathObjectPtr context_res = xmlXPathEvalExpression((xmlChar*) "//*[local-name()='context']", context);
  xmlNodeSetPtr context_nodeset = context_res->nodesetval;
  int context_nodeset_ln = context_nodeset->nodeNr;
  xmlXPathFreeContext(context);

  // Context table (one row per context)
  CharacterVector contextId(context_nodeset_ln);
  CharacterVector scheme(context_nodeset_ln);
  CharacterVector identifier(context_nodeset_ln);
  CharacterVector startDate(context_nodeset_ln);
  CharacterVector endDate(context_nodeset_ln);

  // Dimension table (normalized; grows as members are found)
  vector<string> dim_ctx, dim_name, dim_value;

  for (int i = 0; i < context_nodeset_ln; i++) {
    xmlNodePtr context_node = context_nodeset->nodeTab[i];
    xmlChar *tmp_str;

    string ctx_id;
    if ((tmp_str = xmlGetProp(context_node, (xmlChar*) "id"))) { 
      ctx_id = string((char*)tmp_str);
      contextId[i] = (char*)tmp_str;
      xmlFree(tmp_str);
    } else {
      contextId[i] = NA_STRING;
    }

    scheme[i] = identifier[i] = startDate[i] = endDate[i] = NA_STRING;

    xmlNodePtr child_node = context_node->xmlChildrenNode;
    while (child_node) {
      if (!xmlStrcmp(child_node->name, (xmlChar*) "entity")) {
        xmlNodePtr gchild_node = child_node->xmlChildrenNode;
        while (gchild_node) {
          if (!xmlStrcmp(gchild_node->name, (xmlChar*) "identifier")) {
            if ((tmp_str = xmlGetProp(gchild_node, (xmlChar*) "scheme"))) {
              scheme[i] = (char*)tmp_str;
              xmlFree(tmp_str);
            }
            if ((tmp_str = xmlNodeListGetString(doc, gchild_node->xmlChildrenNode, 1))) {
              identifier[i] = (char*)tmp_str;
              xmlFree(tmp_str);
            }
          } else if (!xmlStrcmp(gchild_node->name, (xmlChar*) "segment")) {
            extract_dims(doc, gchild_node, ctx_id, dim_ctx, dim_name, dim_value);
          }
          gchild_node = gchild_node->next;
        }
      } else if (!xmlStrcmp(child_node->name, (xmlChar*) "period")) {
        xmlNodePtr gchild_node = child_node->xmlChildrenNode;
        while (gchild_node) {
          if (!xmlStrcmp(gchild_node->name, (xmlChar*) "startDate")) {
            if ((tmp_str = xmlNodeListGetString(doc, gchild_node->xmlChildrenNode, 1))) {
              startDate[i] = (char*)tmp_str;
              xmlFree(tmp_str);
            }
          } else if (!xmlStrcmp(gchild_node->name, (xmlChar*) "endDate")) {
            if ((tmp_str = xmlNodeListGetString(doc, gchild_node->xmlChildrenNode, 1))) {
              endDate[i] = (char*)tmp_str;
              xmlFree(tmp_str);
            }
          } else if (!xmlStrcmp(gchild_node->name, (xmlChar*) "instant")) {
            if ((tmp_str = xmlNodeListGetString(doc, gchild_node->xmlChildrenNode, 1))) {
              endDate[i] = (char*)tmp_str;
              xmlFree(tmp_str);
            }
          }
          gchild_node = gchild_node->next;
        }
      } else if (!xmlStrcmp(child_node->name, (xmlChar*) "scenario")) {
        extract_dims(doc, child_node, ctx_id, dim_ctx, dim_name, dim_value);
      }
      child_node = child_node->next;
    }
  }
  xmlXPathFreeObject(context_res);

  int ndims = (int)dim_ctx.size();
  CharacterVector dim_ctx_cv(ndims), dim_name_cv(ndims), dim_value_cv(ndims);
  for (int i = 0; i < ndims; i++) {
    dim_ctx_cv[i]   = dim_ctx[i];
    dim_name_cv[i]  = dim_name[i];
    dim_value_cv[i] = dim_value[i];
  }

  return List::create(
    Named("context") = DataFrame::create(
      Named("contextId")  = contextId,
      Named("scheme")     = scheme,
      Named("identifier") = identifier,
      Named("startDate")  = startDate,
      Named("endDate")    = endDate
    ),
    Named("dimension") = DataFrame::create(
      Named("contextId") = dim_ctx_cv,
      Named("dimension") = dim_name_cv,
      Named("value")     = dim_value_cv
    )
  );
}
