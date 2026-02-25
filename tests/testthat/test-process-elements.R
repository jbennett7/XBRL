test_that("xbrlProcessElements does not crash when no schema element present", {
  fixture <- test_path("fixtures/no_schema_element.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessElements", doc, PACKAGE = "XBRL")
  expect_true(is.data.frame(result))
})
