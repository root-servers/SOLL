// RUN: %soll -lang=Yul %s
// REQUIRES: address
// REQUIRES: delegatecall
// REQUIRES: gas
{
  sstore(0, delegatecall(gas(), address(), 0, 0x20, 0x20, 0x20))
}
// ----
// Trace:
//   INVALID()
// Memory dump:
//      0: 0000000000000000000000000000000011111111000000000000000000000000
// Storage dump:
