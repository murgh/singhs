/*
module XOR2_X2 (A, B, Z);
 input A, B;
 output Z;
 	and a (A, B, Z);
endmodule 
*/
`include <master.v>
module top(net_1 , net_2 , net_3 , net_4);
input  net_1, net_2 ,  net_3;
output net_4;

XOR2_X2 xor_1(.A(net_1) , .B(net_2) , .Z(netn));
XOR2_X2 xor_2(.A(netn) , .B(net_3) , .Z(net_4));

endmodule

