// RUN: %soll -lang=Yul %s
// REQUIRES: create
// REQUIRES: msize
{
  for {} div(create(0, 1, 0), shl(msize(), 1)) {}
  {
  }
}
// ====
// step: expressionSimplifier
// EVMVersion: >byzantium
// ----
// {
//     for { } div(create(0, 1, 0), shl(msize(), 1)) { }
//     { }
// }
