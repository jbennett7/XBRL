test_that("facts with no XML namespace do not crash", {
    fixture <- test_path("fixtures/null_ns_instance.xml")
    doc <- xbrlParse(fixture)
    on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
    result <- .Call("xbrlProcessFacts", doc, PACKAGE = "XBRL")
    expect_true(is.data.frame(result))
    expect_equal(nrow(result), 1)
    expect_equal(result$contextId[1], "ctx1")
    expect_true(is.na(result$ns[1]))
})

