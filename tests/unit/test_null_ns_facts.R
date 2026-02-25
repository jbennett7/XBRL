library(XBRL)
library(testthat)

fixture <- file.path(".", "tests", "fixtures", "null_ns_instance.xml")
doc <- xbrlParse(fixture)

test_that("facts with no XML namespace do not crash", {
    result <- .Call("xbrlProcessFacts", doc, PACKAGE = "XBRL")
    expect_true(is.data.frame(result))
    expect_equal(nrow(result), 1)
    expect_equal(result$contextId[1], "ctx1")
    expect_true(is.na(result$ns[1]))   # no namespace -> NA, not a crash
})

.Call("xbrlFree", doc, PACKAGE = "XBRL")
