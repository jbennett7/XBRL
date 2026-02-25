test_that("xbrlParse stops cleanly on malformed XML", {
    fixture <- test_path("fixtures/malformed.xml")
    expect_error(
        xbrlParse(fixture),
        "Failed to parse"
    )
})
