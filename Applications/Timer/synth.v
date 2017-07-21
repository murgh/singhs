/* Generated by Yosys 0.5+292 (git sha1 6f9a6fd, gcc 4.6.3-1ubuntu5 -fPIC -Os) */

`include <master.v>
module INV_X1;
  input A;
  output ZN;
endmodule
module NOR2_X1;
  input A1, A2;
  output ZN;
endmodule
module OR2_X1;
  input A1, A2;
  output ZN;
endmodule
module DFFRS_X1;
  input D, CK;
  output Q;
endmodule

(* top =  1  *)
(* src = "encoder.v:1" *)
module top(in, clk, o, out);
  (* src = "encoder.v:6" *)
  wire [1:0] _00_;
  (* src = "encoder.v:6" *)
  wire [1:0] _01_;
  wire _02_;
  wire _03_;
  wire _04_;
  wire _05_;
  wire _06_;
  wire _07_;
  wire _08_;
  wire _09_;
  wire _10_;
  wire _11_;
  wire _12_;
  (* src = "encoder.v:3" *)
  input [0:3] in;
  input clk;
  (* src = "encoder.v:4" *)
  output [0:1] out;
  output o;
  INV_X1 _13_ (
    .A(in[3]),
    .ZN(_09_)
  );
  NOR2_X1 _14_ (
    .A1(_09_),
    .A2(in[2]),
    .ZN(_11_)
  );
  NOR2_X1 _15_ (
    .A1(_11_),
    .A2(in[1]),
    .ZN(_12_)
  );
  NOR2_X1 _16_ (
    .A1(_12_),
    .A2(in[0]),
    .ZN(out[1])
  );
  NOR2_X1 _17_ (
    .A1(in[3]),
    .A2(in[2]),
    .ZN(_04_)
  );
  OR2_X1 _18_ (
    .A1(in[0]),
    .A2(in[1]),
    .ZN(_05_)
  );
  NOR2_X1 _19_ (
    .A1(_05_),
    .A2(_04_),
    .ZN(out[0])
  );
  INV_X1 K_ (.A(clk), .ZN(wclk));
  DFFRS_X1 dff (.D(out[0]), .CK(wclk), .Q(o));
endmodule