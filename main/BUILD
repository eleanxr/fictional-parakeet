cc_library(
  name = "libpassman",
  srcs = [ "Key.cpp" ],
  hdrs = [ "Key.h" ],
  visibility = [ "//visibility:public" ],
)

cc_binary(
  name = "passman",
  srcs = [ "main.cpp" ],
  deps = [ "@gcrypt//:gcrypt", "libpassman" ]
)
