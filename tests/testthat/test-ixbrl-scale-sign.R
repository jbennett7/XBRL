test_that("iXBRL scale and sign are applied to produce true XBRL fact values", {
  fixture <- test_path("fixtures/ixbrl_scale_sign.xml")
  doc     <- xbrlParse(fixture)
  on.exit(.Call("xbrlFree", doc, PACKAGE = "XBRL"))
  result <- .Call("xbrlProcessFacts", doc, PACKAGE = "XBRL")

  expect_true(is.data.frame(result))
  expect_equal(nrow(result), 5)

  row <- function(id) result[result$factId == id,]

  # scale=3 only: 1730 * 1000 = 1730000
  expect_equal(row("f1")$fact, "1730000")
  expect_true(is.na(row("f1")$sign))

  # sign="-" only: -100
  expect_equal(row("f2")$fact, "-100")
  expect_true(is.na(row("f2")$sign))

  # scale=3 and sign="-": 293 * 1000 = 293000, negated = -293000
  expect_equal(row("f3")$fact, "-293000")
  expect_true(is.na(row("f3")$sign))

  # no scale or sign: unchanged
  expect_equal(row("f4")$fact, "500000")
  expect_true(is.na(row("f4")$sign)) # sign stays NA

  # comma-formatted + scale: strips commas, 1730 * 1000 = 1730000
  expect_equal(row("f5")$fact, "1730000")
  expect_true(is.na(row("f5")$sign))
})
