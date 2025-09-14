# Test to ensure that a modern inlineXBRL file is read properly.
require(XBRL)
require(testthat)

hlink <- "https://www.sec.gov/Archives/edgar/data/1512673/000151267321000008/sq-20201231.htm"


ua <- 'jbennett@jbennettconsulting.com'
options(HTTPUserAgent = ua)
inst <- xbrlDoAll(hlink)

test_that(desc = "Test that the file was downloaed correctly", code = {
    expect_that(class(inst) == "list", condition = equals(TRUE))
})

test_that(desc = "Fact elementIds conform to the elements", code = {
    check_elements <- unique(inst$fact$elementId)
    elements <- inst$element$elementId
    expect_that(all(check_elements %in% elements), condition = equals(TRUE))
})
