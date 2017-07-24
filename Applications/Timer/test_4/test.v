/* Generated by Yosys 0.5+292 (git sha1 6f9a6fd, gcc 4.6.3-1ubuntu5 -fPIC -Os) */

`include <master.v>

module top(in , clk, out);
input  in, clk;
output out;

BUF_X1 dbuf1_in (.A(in), .Z(w1));
BUF_X1 dbuf2_in (.A(w1), .Z(w3));

BUF_X1 dbuf1_out (.A(w2), .Z(w4));

DFFRS_X1 dff (.D(w1), .CK(wclk), .Q(w2));

AND2_X1 dAnd (.A1(w3), .A2(w4), .ZN(out));

BUF_X1 cbuf (.A(clk), .Z(wclk));

endmodule
