test_that("xbrlDoAll parses minimal fixture end-to-end", {
  inst <- test_path("fixtures/minimal/instance.xml")
  result <- xbrlDoAll(inst, cache.dir=NULL)

  # fact
  expect_equal(nrow(result$fact), 1)
  expect_equal(result$fact$elementId, "acme_Revenue")
  expect_equal(result$fact$contextId, "ctx_2023")
  expect_equal(result$fact$unitId,    "USD")
  expect_equal(result$fact$fact,      "1000000")
  expect_equal(result$fact$decimals,  "-3")

  # context
  expect_equal(nrow(result$context), 1)
  expect_equal(result$context$contextId,  "ctx_2023")
  expect_equal(result$context$identifier, "0001234567")
  expect_equal(result$context$startDate,  "2023-01-01")
  expect_equal(result$context$endDate,    "2023-12-31")

  # dimension (minimal fixture has no dimensional contexts)
  expect_true(!is.null(result$dimension))
  expect_equal(nrow(result$dimension), 0)
  expect_equal(names(result$dimension), c("contextId", "dimension", "value"))

  # unit
  expect_equal(nrow(result$unit), 1)
  expect_equal(result$unit$unitId,  "USD")
  expect_equal(result$unit$measure, "iso4217:USD")

  # element
  expect_equal(nrow(result$element), 1)
  expect_equal(result$element$elementId,  "acme_Revenue")
  expect_equal(result$element$periodType, "duration")
  expect_equal(result$element$balance,    "credit")

  # label
  expect_equal(nrow(result$label), 1)
  expect_equal(result$label$elementId,   "acme_Revenue")
  expect_equal(result$label$labelString, "Revenue")
  expect_equal(result$label$lang,        "en")
})
