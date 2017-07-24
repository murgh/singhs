/* Generated by Yosys 0.5+292 (git sha1 6f9a6fd, gcc 4.6.3-1ubuntu5 -fPIC -Os) */

`include <master.v>

module top(in , clk, out);
input  [0:1] in; 
input  clk;
output [0:1] out;

BUF_X1 db11 (.A(in[0]), .Z(w11));
BUF_X1 db12 (.A(in[0]), .Z(w12));
BUF_X1 db13 (.A(w11), .Z(w13));
DFFRS_X1 dff1 (.D(w11), .CK(wc1), .Q(w14));
BUF_X1 db14 (.A(w14), .Z(w15));
AND2_X1 dbAnd1 (.A1(w15), .A2(w12), .ZN(w16));
OR2_X1 dbor1 (.A1(w13), .A2(w16), .ZN(out[0]));

BUF_X1 db21 (.A(in[1]), .Z(w21));
BUF_X1 db22 (.A(in[1]), .Z(w22));
BUF_X1 db23 (.A(w21), .Z(w15));
BUF_X1 db24 (.A(w21), .Z(w23));
BUF_X1 db25 (.A(w15), .Z(w24));
DFFRS_X1 dff2 (.D(w24), .CK(wc2), .Q(w25));
BUF_X1 db26 (.A(w25), .Z(w26));
AND2_X1 dbAnd2 (.A1(w23), .A2(w22), .ZN(w27));
AND2_X1 dbAnd3 (.A1(w26), .A2(w27), .ZN(out[1]));

/*Clock Path*/
BUF_X1 cb1 (.A(clk), .Z(wc1));
BUF_X1 cb2 (.A(clk), .Z(wc2));

endmodule
