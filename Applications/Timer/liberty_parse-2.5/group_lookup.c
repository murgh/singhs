/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf -L ANSI-C -t -H group_hash_func -N lookup_group_name -C -I -k '*' -D group_lookup  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "group_lookup"

#include "group_enum.h"
#line 4 "group_lookup"
struct libGroupMap { char *name; group_enum type; };
#include <string.h>

#define TOTAL_KEYWORDS 164
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 43
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 933
/* maximum key range = 931, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
group_hash_func (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934,   5,
        0, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934,   0, 934,   0, 135,   5,
      100,   0,  70,  55,  65,   0, 934, 195,  10,  25,
        0,   0,   0,   5,   0,   0,   5,   5, 180,  50,
       90,  55,   5, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934, 934, 934, 934, 934,
      934, 934, 934, 934, 934, 934
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[42]];
      /*FALLTHROUGH*/
      case 42:
        hval += asso_values[(unsigned char)str[41]];
      /*FALLTHROUGH*/
      case 41:
        hval += asso_values[(unsigned char)str[40]];
      /*FALLTHROUGH*/
      case 40:
        hval += asso_values[(unsigned char)str[39]];
      /*FALLTHROUGH*/
      case 39:
        hval += asso_values[(unsigned char)str[38]];
      /*FALLTHROUGH*/
      case 38:
        hval += asso_values[(unsigned char)str[37]];
      /*FALLTHROUGH*/
      case 37:
        hval += asso_values[(unsigned char)str[36]];
      /*FALLTHROUGH*/
      case 36:
        hval += asso_values[(unsigned char)str[35]];
      /*FALLTHROUGH*/
      case 35:
        hval += asso_values[(unsigned char)str[34]];
      /*FALLTHROUGH*/
      case 34:
        hval += asso_values[(unsigned char)str[33]];
      /*FALLTHROUGH*/
      case 33:
        hval += asso_values[(unsigned char)str[32]];
      /*FALLTHROUGH*/
      case 32:
        hval += asso_values[(unsigned char)str[31]];
      /*FALLTHROUGH*/
      case 31:
        hval += asso_values[(unsigned char)str[30]];
      /*FALLTHROUGH*/
      case 30:
        hval += asso_values[(unsigned char)str[29]];
      /*FALLTHROUGH*/
      case 29:
        hval += asso_values[(unsigned char)str[28]];
      /*FALLTHROUGH*/
      case 28:
        hval += asso_values[(unsigned char)str[27]];
      /*FALLTHROUGH*/
      case 27:
        hval += asso_values[(unsigned char)str[26]];
      /*FALLTHROUGH*/
      case 26:
        hval += asso_values[(unsigned char)str[25]];
      /*FALLTHROUGH*/
      case 25:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      /*FALLTHROUGH*/
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct libGroupMap *
lookup_group_name (register const char *str, register unsigned int len)
{
  static const struct libGroupMap wordlist[] =
    {
#line 155 "group_lookup"
      {"pin",	LIBERTY_GROUPENUM_pin},
#line 140 "group_lookup"
      {"seq",	LIBERTY_GROUPENUM_seq},
#line 98 "group_lookup"
      {"upper",	LIBERTY_GROUPENUM_upper},
#line 138 "group_lookup"
      {"state",	LIBERTY_GROUPENUM_state},
#line 145 "group_lookup"
      {"lut",	LIBERTY_GROUPENUM_lut},
#line 49 "group_lookup"
      {"rise_transition",	LIBERTY_GROUPENUM_rise_transition},
#line 21 "group_lookup"
      {"sensitization",	LIBERTY_GROUPENUM_sensitization},
#line 168 "group_lookup"
      {"cell",	LIBERTY_GROUPENUM_cell},
#line 51 "group_lookup"
      {"rise_constraint",	LIBERTY_GROUPENUM_rise_constraint},
#line 102 "group_lookup"
      {"capacitance",	LIBERTY_GROUPENUM_capacitance},
#line 74 "group_lookup"
      {"init_current",	LIBERTY_GROUPENUM_init_current},
#line 87 "group_lookup"
      {"cell_rise",	LIBERTY_GROUPENUM_cell_rise},
#line 103 "group_lookup"
      {"pin_capacitance",	LIBERTY_GROUPENUM_pin_capacitance},
#line 97 "group_lookup"
      {"rise_capacitance",	LIBERTY_GROUPENUM_rise_capacitance},
#line 32 "group_lookup"
      {"intrinsic_parasitic",	LIBERTY_GROUPENUM_intrinsic_parasitic},
#line 31 "group_lookup"
      {"intrinsic_resistance",	LIBERTY_GROUPENUM_intrinsic_resistance},
#line 136 "group_lookup"
      {"test_cell",	LIBERTY_GROUPENUM_test_cell},
#line 7 "group_lookup"
      {"user_parameters",	LIBERTY_GROUPENUM_user_parameters},
#line 30 "group_lookup"
      {"intrinsic_capacitance",	LIBERTY_GROUPENUM_intrinsic_capacitance},
#line 68 "group_lookup"
      {"output_current_rise",	LIBERTY_GROUPENUM_output_current_rise},
#line 161 "group_lookup"
      {"power",	LIBERTY_GROUPENUM_power},
#line 86 "group_lookup"
      {"cell_rise_to_pct",	LIBERTY_GROUPENUM_cell_rise_to_pct},
#line 160 "group_lookup"
      {"rise_power",	LIBERTY_GROUPENUM_rise_power},
#line 120 "group_lookup"
      {"pg_pin",	LIBERTY_GROUPENUM_pg_pin},
#line 135 "group_lookup"
      {"type",	LIBERTY_GROUPENUM_type},
#line 99 "group_lookup"
      {"lower",	LIBERTY_GROUPENUM_lower},
#line 77 "group_lookup"
      {"compact_ccs_rise",	LIBERTY_GROUPENUM_compact_ccs_rise},
#line 52 "group_lookup"
      {"retaining_rise",	LIBERTY_GROUPENUM_retaining_rise},
#line 50 "group_lookup"
      {"rise_propagation",	LIBERTY_GROUPENUM_rise_propagation},
#line 163 "group_lookup"
      {"internal_power",	LIBERTY_GROUPENUM_internal_power},
#line 33 "group_lookup"
      {"pg_current",	LIBERTY_GROUPENUM_pg_current},
#line 54 "group_lookup"
      {"retain_rise_slew",	LIBERTY_GROUPENUM_retain_rise_slew},
#line 16 "group_lookup"
      {"noise_lut_template",	LIBERTY_GROUPENUM_noise_lut_template},
#line 148 "group_lookup"
      {"latch",	LIBERTY_GROUPENUM_latch},
#line 154 "group_lookup"
      {"timing",	LIBERTY_GROUPENUM_timing},
#line 153 "group_lookup"
      {"tlatch",	LIBERTY_GROUPENUM_tlatch},
#line 96 "group_lookup"
      {"rise_capacitance_range",	LIBERTY_GROUPENUM_rise_capacitance_range},
#line 104 "group_lookup"
      {"ccsn_last_stage",	LIBERTY_GROUPENUM_ccsn_last_stage},
#line 14 "group_lookup"
      {"output_current_template",	LIBERTY_GROUPENUM_output_current_template},
#line 133 "group_lookup"
      {"em_lut_template",	LIBERTY_GROUPENUM_em_lut_template},
#line 144 "group_lookup"
      {"memory",	LIBERTY_GROUPENUM_memory},
#line 83 "group_lookup"
      {"fall_transition",	LIBERTY_GROUPENUM_fall_transition},
#line 85 "group_lookup"
      {"fall_constraint",	LIBERTY_GROUPENUM_fall_constraint},
#line 166 "group_lookup"
      {"electromigration",	LIBERTY_GROUPENUM_electromigration},
#line 134 "group_lookup"
      {"poly_template",	LIBERTY_GROUPENUM_poly_template},
#line 89 "group_lookup"
      {"cell_fall",	LIBERTY_GROUPENUM_cell_fall},
#line 20 "group_lookup"
      {"compact_lut_template",	LIBERTY_GROUPENUM_compact_lut_template},
#line 101 "group_lookup"
      {"fall_capacitance",	LIBERTY_GROUPENUM_fall_capacitance},
#line 114 "group_lookup"
      {"max_cap",	LIBERTY_GROUPENUM_max_cap},
#line 113 "group_lookup"
      {"max_trans",	LIBERTY_GROUPENUM_max_trans},
#line 119 "group_lookup"
      {"dc_current",	LIBERTY_GROUPENUM_dc_current},
#line 164 "group_lookup"
      {"domain",	LIBERTY_GROUPENUM_domain},
#line 127 "group_lookup"
      {"power_supply",	LIBERTY_GROUPENUM_power_supply},
#line 128 "group_lookup"
      {"power_lut_template",	LIBERTY_GROUPENUM_power_lut_template},
#line 13 "group_lookup"
      {"pg_current_template",	LIBERTY_GROUPENUM_pg_current_template},
#line 131 "group_lookup"
      {"model",	LIBERTY_GROUPENUM_model},
#line 151 "group_lookup"
      {"ff",	LIBERTY_GROUPENUM_ff},
#line 152 "group_lookup"
      {"bus",	LIBERTY_GROUPENUM_bus},
#line 78 "group_lookup"
      {"output_current_fall",	LIBERTY_GROUPENUM_output_current_fall},
#line 88 "group_lookup"
      {"cell_fall_to_pct",	LIBERTY_GROUPENUM_cell_fall_to_pct},
#line 10 "group_lookup"
      {"propagation_lut_template",	LIBERTY_GROUPENUM_propagation_lut_template},
#line 162 "group_lookup"
      {"fall_power",	LIBERTY_GROUPENUM_fall_power},
#line 126 "group_lookup"
      {"scaled_cell",	LIBERTY_GROUPENUM_scaled_cell},
#line 124 "group_lookup"
      {"timing_range",	LIBERTY_GROUPENUM_timing_range},
#line 76 "group_lookup"
      {"compact_ccs_fall",	LIBERTY_GROUPENUM_compact_ccs_fall},
#line 110 "group_lookup"
      {"ccsn_first_stage",	LIBERTY_GROUPENUM_ccsn_first_stage},
#line 53 "group_lookup"
      {"retaining_fall",	LIBERTY_GROUPENUM_retaining_fall},
#line 125 "group_lookup"
      {"scaling_factors",	LIBERTY_GROUPENUM_scaling_factors},
#line 84 "group_lookup"
      {"fall_propagation",	LIBERTY_GROUPENUM_fall_propagation},
#line 123 "group_lookup"
      {"wire_load",	LIBERTY_GROUPENUM_wire_load},
#line 137 "group_lookup"
      {"statetable",	LIBERTY_GROUPENUM_statetable},
#line 55 "group_lookup"
      {"retain_fall_slew",	LIBERTY_GROUPENUM_retain_fall_slew},
#line 158 "group_lookup"
      {"memory_write",	LIBERTY_GROUPENUM_memory_write},
#line 11 "group_lookup"
      {"power_poly_template",	LIBERTY_GROUPENUM_power_poly_template},
#line 9 "group_lookup"
      {"rise_net_delay",	LIBERTY_GROUPENUM_rise_net_delay},
#line 100 "group_lookup"
      {"fall_capacitance_range",	LIBERTY_GROUPENUM_fall_capacitance_range},
#line 130 "group_lookup"
      {"operating_conditions",	LIBERTY_GROUPENUM_operating_conditions},
#line 70 "group_lookup"
      {"left_id",	LIBERTY_GROUPENUM_left_id},
#line 79 "group_lookup"
      {"noise_immunity_low",	LIBERTY_GROUPENUM_noise_immunity_low},
#line 156 "group_lookup"
      {"minimum_period",	LIBERTY_GROUPENUM_minimum_period},
#line 108 "group_lookup"
      {"vector",	LIBERTY_GROUPENUM_vector},
#line 112 "group_lookup"
      {"input_signal_swing",	LIBERTY_GROUPENUM_input_signal_swing},
#line 122 "group_lookup"
      {"wire_load_selection",	LIBERTY_GROUPENUM_wire_load_selection},
#line 6 "group_lookup"
      {"interconnect_delay",	LIBERTY_GROUPENUM_interconnect_delay},
#line 18 "group_lookup"
      {"maxcap_lut_template",	LIBERTY_GROUPENUM_maxcap_lut_template},
#line 17 "group_lookup"
      {"maxtrans_lut_template",	LIBERTY_GROUPENUM_maxtrans_lut_template},
#line 169 "group_lookup"
      {"library",	LIBERTY_GROUPENUM_library},
#line 111 "group_lookup"
      {"output_signal_swing",	LIBERTY_GROUPENUM_output_signal_swing},
#line 38 "group_lookup"
      {"va_rise_constraint",	LIBERTY_GROUPENUM_va_rise_constraint},
#line 35 "group_lookup"
      {"dynamic_current",	LIBERTY_GROUPENUM_dynamic_current},
#line 159 "group_lookup"
      {"memory_read",	LIBERTY_GROUPENUM_memory_read},
#line 73 "group_lookup"
      {"peak_current",	LIBERTY_GROUPENUM_peak_current},
#line 95 "group_lookup"
      {"receiver_capacitance",	LIBERTY_GROUPENUM_receiver_capacitance},
#line 19 "group_lookup"
      {"lu_table_template",	LIBERTY_GROUPENUM_lu_table_template},
#line 46 "group_lookup"
      {"steady_state_current_tristate",	LIBERTY_GROUPENUM_steady_state_current_tristate},
#line 91 "group_lookup"
      {"receiver_capacitance2_rise",	LIBERTY_GROUPENUM_receiver_capacitance2_rise},
#line 69 "group_lookup"
      {"right_id",	LIBERTY_GROUPENUM_right_id},
#line 71 "group_lookup"
      {"peak_time",	LIBERTY_GROUPENUM_peak_time},
#line 93 "group_lookup"
      {"receiver_capacitance1_rise",	LIBERTY_GROUPENUM_receiver_capacitance1_rise},
#line 106 "group_lookup"
      {"propagated_noise_low",	LIBERTY_GROUPENUM_propagated_noise_low},
#line 12 "group_lookup"
      {"ccs_lu_table_template",	LIBERTY_GROUPENUM_ccs_lu_table_template},
#line 44 "group_lookup"
      {"va_compact_ccs_rise",	LIBERTY_GROUPENUM_va_compact_ccs_rise},
#line 75 "group_lookup"
      {"compressed_ccs_timing_rise",	LIBERTY_GROUPENUM_compressed_ccs_timing_rise},
#line 34 "group_lookup"
      {"switching_group",	LIBERTY_GROUPENUM_switching_group},
#line 167 "group_lookup"
      {"bundle",	LIBERTY_GROUPENUM_bundle},
#line 25 "group_lookup"
      {"iv_lut_template",	LIBERTY_GROUPENUM_iv_lut_template},
#line 47 "group_lookup"
      {"steady_state_current_low",	LIBERTY_GROUPENUM_steady_state_current_low},
#line 132 "group_lookup"
      {"input_voltage",	LIBERTY_GROUPENUM_input_voltage},
#line 27 "group_lookup"
      {"fall_net_delay",	LIBERTY_GROUPENUM_fall_net_delay},
#line 157 "group_lookup"
      {"min_pulse_width",	LIBERTY_GROUPENUM_min_pulse_width},
#line 141 "group_lookup"
      {"routing_track",	LIBERTY_GROUPENUM_routing_track},
#line 129 "group_lookup"
      {"output_voltage",	LIBERTY_GROUPENUM_output_voltage},
#line 165 "group_lookup"
      {"em_max_toggle_rate",	LIBERTY_GROUPENUM_em_max_toggle_rate},
#line 109 "group_lookup"
      {"output_voltage_rise",	LIBERTY_GROUPENUM_output_voltage_rise},
#line 29 "group_lookup"
      {"leakage_current",	LIBERTY_GROUPENUM_leakage_current},
#line 8 "group_lookup"
      {"rise_transition_degradation",	LIBERTY_GROUPENUM_rise_transition_degradation},
#line 90 "group_lookup"
      {"cell_degradation",	LIBERTY_GROUPENUM_cell_degradation},
#line 37 "group_lookup"
      {"va_fall_constraint",	LIBERTY_GROUPENUM_va_fall_constraint},
#line 143 "group_lookup"
      {"mode_definition",	LIBERTY_GROUPENUM_mode_definition},
#line 80 "group_lookup"
      {"noise_immunity_high",	LIBERTY_GROUPENUM_noise_immunity_high},
#line 92 "group_lookup"
      {"receiver_capacitance2_fall",	LIBERTY_GROUPENUM_receiver_capacitance2_fall},
#line 146 "group_lookup"
      {"leakage_power",	LIBERTY_GROUPENUM_leakage_power},
#line 121 "group_lookup"
      {"wire_load_table",	LIBERTY_GROUPENUM_wire_load_table},
#line 94 "group_lookup"
      {"receiver_capacitance1_fall",	LIBERTY_GROUPENUM_receiver_capacitance1_fall},
#line 142 "group_lookup"
      {"mode_value",	LIBERTY_GROUPENUM_mode_value},
#line 28 "group_lookup"
      {"gate_leakage",	LIBERTY_GROUPENUM_gate_leakage},
#line 22 "group_lookup"
      {"base_curves",	LIBERTY_GROUPENUM_base_curves},
#line 43 "group_lookup"
      {"va_compact_ccs_fall",	LIBERTY_GROUPENUM_va_compact_ccs_fall},
#line 139 "group_lookup"
      {"seq_bank",	LIBERTY_GROUPENUM_seq_bank},
#line 115 "group_lookup"
      {"hyperbolic_noise_low",	LIBERTY_GROUPENUM_hyperbolic_noise_low},
#line 105 "group_lookup"
      {"propagated_noise_high",	LIBERTY_GROUPENUM_propagated_noise_high},
#line 107 "group_lookup"
      {"output_voltage_fall",	LIBERTY_GROUPENUM_output_voltage_fall},
#line 26 "group_lookup"
      {"fall_transition_degradation",	LIBERTY_GROUPENUM_fall_transition_degradation},
#line 149 "group_lookup"
      {"generated_clock",	LIBERTY_GROUPENUM_generated_clock},
#line 81 "group_lookup"
      {"noise_immunity_below_low",	LIBERTY_GROUPENUM_noise_immunity_below_low},
#line 48 "group_lookup"
      {"steady_state_current_high",	LIBERTY_GROUPENUM_steady_state_current_high},
#line 39 "group_lookup"
      {"va_receiver_capacitance2_rise",	LIBERTY_GROUPENUM_va_receiver_capacitance2_rise},
#line 41 "group_lookup"
      {"va_receiver_capacitance1_rise",	LIBERTY_GROUPENUM_va_receiver_capacitance1_rise},
#line 147 "group_lookup"
      {"latch_bank",	LIBERTY_GROUPENUM_latch_bank},
#line 64 "group_lookup"
      {"propagated_noise_height_low",	LIBERTY_GROUPENUM_propagated_noise_height_low},
#line 36 "group_lookup"
      {"pin_based_variation",	LIBERTY_GROUPENUM_pin_based_variation},
#line 23 "group_lookup"
      {"ccs_timing_base_curve",	LIBERTY_GROUPENUM_ccs_timing_base_curve},
#line 72 "group_lookup"
      {"peak_voltage",	LIBERTY_GROUPENUM_peak_voltage},
#line 56 "group_lookup"
      {"propagated_noise_width_low",	LIBERTY_GROUPENUM_propagated_noise_width_low},
#line 116 "group_lookup"
      {"hyperbolic_noise_high",	LIBERTY_GROUPENUM_hyperbolic_noise_high},
#line 150 "group_lookup"
      {"ff_bank",	LIBERTY_GROUPENUM_ff_bank},
#line 60 "group_lookup"
      {"propagated_noise_peak_time_ratio_low",	LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_low},
#line 24 "group_lookup"
      {"ccs_timing_base_curve_template",	LIBERTY_GROUPENUM_ccs_timing_base_curve_template},
#line 40 "group_lookup"
      {"va_receiver_capacitance2_fall",	LIBERTY_GROUPENUM_va_receiver_capacitance2_fall},
#line 42 "group_lookup"
      {"va_receiver_capacitance1_fall",	LIBERTY_GROUPENUM_va_receiver_capacitance1_fall},
#line 45 "group_lookup"
      {"timing_based_variation",	LIBERTY_GROUPENUM_timing_based_variation},
#line 117 "group_lookup"
      {"hyperbolic_noise_below_low",	LIBERTY_GROUPENUM_hyperbolic_noise_below_low},
#line 65 "group_lookup"
      {"propagated_noise_height_high",	LIBERTY_GROUPENUM_propagated_noise_height_high},
#line 57 "group_lookup"
      {"propagated_noise_width_high",	LIBERTY_GROUPENUM_propagated_noise_width_high},
#line 61 "group_lookup"
      {"propagated_noise_peak_time_ratio_high",	LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_high},
#line 66 "group_lookup"
      {"propagated_noise_height_below_low",	LIBERTY_GROUPENUM_propagated_noise_height_below_low},
#line 82 "group_lookup"
      {"noise_immunity_above_high",	LIBERTY_GROUPENUM_noise_immunity_above_high},
#line 58 "group_lookup"
      {"propagated_noise_width_below_low",	LIBERTY_GROUPENUM_propagated_noise_width_below_low},
#line 62 "group_lookup"
      {"propagated_noise_peak_time_ratio_below_low",	LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_below_low},
#line 15 "group_lookup"
      {"normalized_driver_waveform",	LIBERTY_GROUPENUM_normalized_driver_waveform},
#line 118 "group_lookup"
      {"hyperbolic_noise_above_high",	LIBERTY_GROUPENUM_hyperbolic_noise_above_high},
#line 67 "group_lookup"
      {"propagated_noise_height_above_high",	LIBERTY_GROUPENUM_propagated_noise_height_above_high},
#line 59 "group_lookup"
      {"propagated_noise_width_above_high",	LIBERTY_GROUPENUM_propagated_noise_width_above_high},
#line 63 "group_lookup"
      {"propagated_noise_peak_time_ratio_above_high",	LIBERTY_GROUPENUM_propagated_noise_peak_time_ratio_above_high}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,   1,  -1,
        2,  -1,  -1,  -1,  -1,   3,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   4,  -1,   5,  -1,  -1,   6,   7,
        8,   9,  10,  -1,  11,  12,  13,  -1,  -1,  14,
       15,  -1,  -1,  -1,  16,  -1,  -1,  -1,  -1,  -1,
       17,  18,  -1,  -1,  19,  20,  21,  -1,  -1,  -1,
       22,  23,  -1,  -1,  24,  25,  26,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  27,  -1,  28,  -1,  -1,  29,
       30,  31,  -1,  32,  -1,  -1,  -1,  -1,  -1,  -1,
       33,  34,  -1,  -1,  -1,  -1,  35,  36,  -1,  -1,
       37,  -1,  -1,  38,  -1,  39,  -1,  -1,  -1,  -1,
       -1,  40,  -1,  -1,  -1,  41,  -1,  -1,  -1,  -1,
       42,  43,  -1,  44,  45,  46,  47,  48,  -1,  49,
       50,  51,  52,  53,  54,  -1,  -1,  -1,  -1,  -1,
       55,  -1,  56,  57,  58,  -1,  59,  -1,  -1,  60,
       61,  62,  63,  -1,  -1,  -1,  64,  -1,  -1,  -1,
       -1,  65,  -1,  -1,  66,  67,  68,  -1,  -1,  69,
       70,  71,  72,  -1,  -1,  -1,  -1,  -1,  -1,  73,
       -1,  -1,  -1,  -1,  74,  -1,  -1,  75,  -1,  -1,
       76,  -1,  77,  78,  79,  -1,  80,  -1,  81,  82,
       -1,  -1,  -1,  83,  84,  -1,  85,  86,  -1,  87,
       -1,  -1,  -1,  88,  -1,  89,  90,  -1,  -1,  -1,
       -1,  -1,  91,  -1,  -1,  92,  -1,  93,  -1,  94,
       -1,  95,  -1,  96,  97,  -1,  98,  -1,  -1,  -1,
       99, 100,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 101,
       -1, 102,  -1,  -1,  -1, 103, 104,  -1,  -1,  -1,
      105,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 106,
       -1,  -1,  -1, 107, 108, 109,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 110, 111,  -1,  -1,  -1, 112, 113,
      114,  -1,  -1,  -1,  -1,  -1,  -1, 115,  -1,  -1,
       -1, 116,  -1, 117,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 118,  -1,  -1,  -1, 119,
       -1, 120,  -1, 121,  -1, 122, 123,  -1,  -1,  -1,
      124,  -1, 125,  -1,  -1,  -1, 126,  -1,  -1, 127,
       -1,  -1,  -1, 128,  -1,  -1,  -1,  -1,  -1,  -1,
      129,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 130,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 131,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 132,  -1,  -1,
      133,  -1,  -1,  -1, 134, 135,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 136,  -1,  -1,  -1,  -1, 137,
       -1,  -1,  -1,  -1,  -1, 138,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 139,  -1, 140,
       -1, 141,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 142,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 143,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 144, 145,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 146,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 147,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 148,  -1,  -1,  -1,  -1, 149,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 150,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 151,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 152,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 153,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 154,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 155,  -1,
      156,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 157,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 158,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 159,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 160,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1, 161,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 162,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 163
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = group_hash_func (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}
#line 170 "group_lookup"

