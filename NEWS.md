# XBRL 1.1.0

* `xbrlProcessContexts()` (and therefore `xbrlDoAll()$dimension`) now returns
  axis and member names in schema-id form (`us-gaap_ServiceMember`) instead of
  QName form (`us-gaap:ServiceMember`), matching `elementId` in the fact,
  element, presentation and label tables. Filters written against the colon
  form will no longer match. Typed-member values are unchanged.
