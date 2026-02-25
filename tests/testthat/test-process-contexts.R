test_that("xbrlProcessContexts returns a list with context and dimension tables", {
  fixture <- test_path("fixtures/multi_dim/instance.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessContexts", doc, PACKAGE = "XBRL")

  expect_true(is.list(result))
  expect_true(is.data.frame(result$context))
  expect_true(is.data.frame(result$dimension))

  # context table: 4 rows, 5 columns (no dimension1..4 wide columns)
  expect_equal(nrow(result$context), 4)
  expect_equal(names(result$context),
    c("contextId", "scheme", "identifier", "startDate", "endDate"))

  # dimension table: 5 (seg5) + 2 (scenario) + 1 (typed) = 8 rows
  expect_equal(nrow(result$dimension), 8)
  expect_equal(names(result$dimension), c("contextId", "dimension", "value"))
})

test_that("xbrlProcessContexts: >4 explicit dimensions in segment all captured", {
  fixture <- test_path("fixtures/multi_dim/instance.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessContexts", doc, PACKAGE = "XBRL")

  dims <- result$dimension[result$dimension$contextId == "ctx_seg5", ]
  expect_equal(nrow(dims), 5)
  expect_equal(sort(dims$dimension),
    c("acme:Dim1", "acme:Dim2", "acme:Dim3", "acme:Dim4", "acme:Dim5"))
  expect_equal(sort(dims$value),
    c("acme:Val1", "acme:Val2", "acme:Val3", "acme:Val4", "acme:Val5"))
})

test_that("xbrlProcessContexts: scenario dimensions captured", {
  fixture <- test_path("fixtures/multi_dim/instance.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessContexts", doc, PACKAGE = "XBRL")

  dims <- result$dimension[result$dimension$contextId == "ctx_scenario", ]
  expect_equal(nrow(dims), 2)
  expect_equal(sort(dims$dimension), c("acme:Dim1", "acme:Dim2"))
  expect_equal(sort(dims$value), c("acme:ScenVal1", "acme:ScenVal2"))
})

test_that("xbrlProcessContexts: typedMember captured", {
  fixture <- test_path("fixtures/multi_dim/instance.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessContexts", doc, PACKAGE = "XBRL")

  dims <- result$dimension[result$dimension$contextId == "ctx_typed", ]
  expect_equal(nrow(dims), 1)
  expect_equal(dims$dimension[1], "acme:TypedDim")
  expect_equal(dims$value[1], "SomeTypedValue")
})

test_that("xbrlProcessContexts: plain context has no dimension rows", {
  fixture <- test_path("fixtures/multi_dim/instance.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessContexts", doc, PACKAGE = "XBRL")

  dims <- result$dimension[result$dimension$contextId == "ctx_plain", ]
  expect_equal(nrow(dims), 0)
})

test_that("xbrlProcessContexts: instant period stored as endDate, startDate is NA", {
  fixture <- test_path("fixtures/multi_dim/instance.xml")
  doc <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessContexts", doc, PACKAGE = "XBRL")

  ctx <- result$context[result$context$contextId == "ctx_seg5", ]
  expect_equal(ctx$endDate, "2023-12-31")
  expect_true(is.na(ctx$startDate))
})
