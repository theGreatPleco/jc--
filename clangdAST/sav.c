/*

🄔  BinaryOperator =
  🄔  ArraySubscript
    🄔  ImplicitCast ArrayToPointerDecay
      🄔  DeclRef c
    🄔  IntegerLiteral 1
  🄔  ImplicitCast IntegralCast
    🄔  IntegerLiteral 2

*/

int main(){
  char c[32];
  c[0] = 1;
  c[1] = 2;
}
