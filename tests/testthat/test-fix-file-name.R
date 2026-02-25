test_that("fixFileName resolves ../ relative paths correctly", {
  inst <- test_path("fixtures/minimal/subdir/instance-subdir.xml")
  result <- xbrlDoAll(inst, cache.dir = NULL)

  expect_equal(nrow(result$fact), 1)
  expect_equal(result$fact$elementId, "acme_Revenue")
  expect_equal(nrow(result$label), 1)
  expect_equal(result$label$labelString, "Revenue")
})
