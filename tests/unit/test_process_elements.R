library(XBRL)
library(testthat)

test_that("xbrlProcessElements does not crash when no schema element present", {
  doc <- xbrlParse("tests/fixtures/no_schema_element.xml")
  result <- .Call("xbrlProcessElements", doc, PACKAGE = "XBRL")
  expect_true(is.data.frame(result))
  .Call("xbrlFree", doc, PACKAGE = "XBRL")
})
