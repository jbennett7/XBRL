library(XBRL)
library(testthat)

test_that("xbrlParse stops cleanly on malformed XML", {
    expect_error(
        xbrlParse("tests/fixtures/malformed.xml"),
        "Failed to parse"
    )
})
