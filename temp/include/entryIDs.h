/*
 * Copyright (c) 1996-2011 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#if !defined(ENTRYIDS_IA32_H)
#define ENTRYIDS_IA32_H

#include "dyntypes.h"

enum entryID {
  e_jb = 0,
  e_jb_jnaej_j,
  e_jbe,
  e_jcxz_jec,
  e_jl,
  e_jle,
  e_jmp,
  e_jmpe,
  e_jnb,
  e_jnb_jae_j,
  e_jnbe,
  e_jnl,
  e_jnle,
  e_jno,
  e_jnp,
  e_jns,
  e_jnz,
  e_jo,
  e_jp,
  e_js,
  e_jz,
  e_loop,
  e_loope,
  e_loopn,
  e_call,
  e_cmp,
  e_cmppd,
  e_cmpps,
  e_cmpsb,
  e_cmpsd,
  e_cmpss,
  e_cmpsw,
  e_cmpxch,
  e_cmpxch8b,
  e_ret_far,
  e_ret_near,
  e_prefetch,
  e_prefetchNTA,
  e_prefetchT0,
  e_prefetchT1,
  e_prefetchT2,
  e_prefetch_w,
  e_prefetchw,
  e_No_Entry,
  e_aaa,
  e_aad,
  e_aam,
  e_aas,
  e_adc,
  e_add,
  e_addpd,
  e_addps,
  e_addsd,
  e_addss,
  e_addsubpd,
  e_addsubps,
  e_and,
  e_andnpd,
  e_andnps,
  e_andpd,
  e_andps,
  e_arpl,
  e_bound,
  e_bsf,
  e_bsr,
  e_bswap,
  e_bt,
  e_btc,
  e_btr,
  e_bts,
  e_cbw,
  e_cdq,
  e_clc,
  e_cld,
  e_clflush,
  e_cli,
  e_clts,
  e_cmc,
  e_cmovbe,
  e_cmove,
  e_cmovnae,
  e_cmovnb,
  e_cmovnbe,
  e_cmovne,
  e_cmovng,
  e_cmovnge,
  e_cmovnl,
  e_cmovno,
  e_cmovns,
  e_cmovo,
  e_cmovpe,
  e_cmovpo,
  e_cmovs,
  e_comisd,
  e_comiss,
  e_cpuid,
  e_cvtdq2pd,
  e_cvtdq2ps,
  e_cvtpd2dq,
  e_cvtpd2pi,
  e_cvtpd2ps,
  e_cvtpi2pd,
  e_cvtpi2ps,
  e_cvtps2dq,
  e_cvtps2pd,
  e_cvtps2pi,
  e_cvtsd2si,
  e_cvtsd2ss,
  e_cvtsi2sd,
  e_cvtsi2ss,
  e_cvtss2sd,
  e_cvtss2si,
  e_cvttpd2dq,
  e_cvttpd2pi,
  e_cvttps2dq,
  e_cvttps2pi,
  e_cvttsd2si,
  e_cvttss2si,
  e_cwd,
  e_cwde,
  e_daa,
  e_das,
  e_dec,
  e_div,
  e_divpd,
  e_divps,
  e_divsd,
  e_divss,
  e_emms,
  e_enter,
  e_extrq,
  e_fadd,
  e_faddp,
  e_f2xm1,
  e_fbld,
  e_fbstp,
  e_fchs,
  e_fcmovb,
  e_fcmovbe,
  e_fcmove,
  e_fcmovne,
  e_fcmovu,
  e_fcmovnu,
  e_fcmovnb,
  e_fcmovnbe,
  e_fcom,
  e_fcomi,
  e_fcomip,
  e_fcomp,
  e_fcompp,
  e_fdiv,
  e_fdivp,
  e_fdivr,
  e_fdivrp,
  e_femms,
  e_ffree,
  e_fiadd,
  e_ficom,
  e_ficomp,
  e_fidiv,
  e_fidivr,
  e_fild,
  e_fimul,
  e_fist,
  e_fistp,
  e_fisttp,
  e_fisub,
  e_fisubr,
  e_fld,
  e_fld1, // note: numeral '1', as in load the constant
  e_fldcw,
  e_fldenv,
  e_fmul,
  e_fmulp,
  e_fnop,
  e_fprem,
  e_frstor,
  e_fsave,
  e_fst,
  e_fstcw,
  e_fstenv,
  e_fstp,
  e_fstsw,
  e_fsub,
  e_fsubp,
  e_fsubr,
  e_fsubrp,
  e_fucom,
  e_fucomp,
  e_fucomi,
  e_fucomip,
  e_fucompp,
  e_fxch,
  e_fxrstor,
  e_fxsave,
  e_haddpd,
  e_haddps,
  e_hlt,
  e_hsubpd,
  e_hsubps,
  e_idiv,
  e_imul,
  e_in,
  e_inc,
  e_insb,
  e_insd,
  e_insertq,
  e_insw,
  e_int,
  e_int3,
  e_int1,
  e_int80,
  e_into,
  e_invd,
  e_invlpg,
  e_iret,
  e_lahf,
  e_lar,
  e_lddqu,
  e_ldmxcsr,
  e_lds,
  e_lea,
  e_leave,
  e_les,
  e_lfence,
  e_lfs,
  e_lgdt,
  e_lgs,
  e_lidt,
  e_lldt,
  e_lmsw,
  e_lodsb,
  e_lodsd,
  e_lodsw,
  e_lsl,
  e_lss,
  e_ltr,
  e_maskmovdqu,
  e_maskmovq,
  e_maxpd,
  e_maxps,
  e_maxsd,
  e_maxss,
  e_mfence,
  e_minpd,
  e_minps,
  e_minsd,
  e_minss,
  e_mmxud,
  e_mov,
  e_movapd,
  e_movaps,
  e_movd,
  e_movddup,
  e_movdq2q,
  e_movdqa,
  e_movdqu,
  e_movhpd,
  e_movhps,
  e_movhps_movlhps,
  e_movlpd,
  e_movlps,
  e_movlps_movhlps,
  e_movmskpd,
  e_movmskps,
  e_movntdq,
  e_movnti,
  e_movntpd,
  e_movntps,
  e_movntq,
  e_movntsd,
  e_movntss,
  e_movq,
  e_movq2dq,
  e_movsb,
  e_movsd,
  e_movshdup,
  e_movsldup,
  e_movss,
  e_movsw,
  e_movsx,
  e_movsxd,
  e_movupd,
  e_movups,
  e_movzx,
  e_mul,
  e_mulpd,
  e_mulps,
  e_mulsd,
  e_mulss,
  e_neg,
  e_nop,
  e_not,
  e_or,
  e_orpd,
  e_orps,
  e_out,
  e_outsb,
  e_outsd,
  e_outsw,
  e_packssdw,
  e_packsswb,
  e_packuswb,
  e_paddb,
  e_paddd,
  e_paddq,
  e_paddsb,
  e_paddsw,
  e_paddusb,
  e_paddusw,
  e_paddw,
  e_pand,
  e_pandn,
  e_pavgb,
  e_pavgw,
  e_pcmpeqb,
  e_pcmpeqd,
  e_pcmpeqw,
  e_pcmpgdt,
  e_pcmpgtb,
  e_pcmpgtw,
  e_pextrw,
  e_pinsrw,
  e_pmaddwd,
  e_pmaxsw,
  e_pmaxub,
  e_pminsw,
  e_pminub,
  e_pmovmskb,
  e_pmulhuw,
  e_pmulhw,
  e_pmullw,
  e_pmuludq,
  e_pop,
  e_popa,
  e_popad,
  e_popf,
  e_popfd,
  e_popcnt,
  e_por,
  e_psadbw,
  e_pshufd,
  e_pshufhw,
  e_pshuflw,
  e_pshufw,
  e_pslld,
  e_pslldq,
  e_psllq,
  e_psllw,
  e_psrad,
  e_psraw,
  e_psrld,
  e_psrldq,
  e_psrlq,
  e_psrlw,
  e_psubb,
  e_psubd,
  e_psubsb,
  e_psubsw,
  e_psubusb,
  e_psubusw,
  e_psubw,
  e_punpckhbw,
  e_punpckhdq,
  e_punpckhqd,
  e_punpckhwd,
  e_punpcklbw,
  e_punpcklqd,
  e_punpcklqld,
  e_punpcklwd,
  e_push,
  e_pusha,
  e_pushad,
  e_pushf,
  e_pushfd,
  e_pxor,
  e_rcl,
  e_rcpps,
  e_rcpss,
  e_rcr,
  e_rdmsr,
  e_rdpmc,
  e_rdtsc,
  e_rol,
  e_ror,
  e_rsm,
  e_rsqrtps,
  e_rsqrtss,
  e_sahf,
  e_salc,
  e_sar,
  e_sbb,
  e_scasb,
  e_scasd,
  e_scasw,
  e_setb,
  e_setbe,
  e_setl,
  e_setle,
  e_setnb,
  e_setnbe,
  e_setnl,
  e_setnle,
  e_setno,
  e_setnp,
  e_setns,
  e_setnz,
  e_seto,
  e_setp,
  e_sets,
  e_setz,
  e_sfence,
  e_sgdt,
  e_shl_sal,
  e_shld,
  e_shr,
  e_shrd,
  e_shufpd,
  e_shufps,
  e_sidt,
  e_sldt,
  e_smsw,
  e_sqrtpd,
  e_sqrtps,
  e_sqrtsd,
  e_sqrtss,
  e_stc,
  e_std,
  e_sti,
  e_stmxcsr,
  e_stosb,
  e_stosd,
  e_stosw,
  e_str,
  e_sub,
  e_subpd,
  e_subps,
  e_subsd,
  e_subss,
  e_syscall,
  e_sysenter,
  e_sysexit,
  e_sysret,
  e_test,
  e_ucomisd,
  e_ucomiss,
  e_ud,
  e_ud2,
  e_ud2grp10,
  e_unpckhpd,
  e_unpckhps,
  e_unpcklpd,
  e_unpcklps,
  e_verr,
  e_verw,
  e_vmread,
  e_vmwrite,
  e_vsyscall,
  e_wait,
  e_wbinvd,
  e_wrmsr,
  e_xadd,
  e_xchg,
  e_xlat,
  e_xor,
  e_xorpd,
  e_xorps,
  e_fp_generic,
  e_3dnow_generic,

  power_op_INVALID,
  power_op_extended,
  power_op_stfdu,
  power_op_fadd,
  power_op_xoris,
  power_op_mulhwu,
  power_op_stbux,
  power_op_cmpl,
  power_op_subf,
  power_op_svcs,
  power_op_fmuls,
  power_op_subfic,
  power_op_mcrfs,
  power_op_divs,
  power_op_lwzx,
  power_op_fctiw,
  power_op_mtcrf,
  power_op_srq,
  power_op_sraw,
  power_op_lfdx,
  power_op_stdcx_rc,
  power_op_nor,
  power_op_crandc,
  power_op_stdu,
  power_op_addme,
  power_op_fmul,
  power_op_sthbrx,
  power_op_mtspr,
  power_op_lfsx,
  power_op_lbzx,
  power_op_nand,
  power_op_fnmadds,
  power_op_fnmadd,
  power_op_mulhw,
  power_op_sradi,
  power_op_fnmsubs,
  power_op_addze,
  power_op_mulld,
  power_op_addic,
  power_op_lfs,
  power_op_andc,
  power_op_eciwx,
  power_op_rfid,
  power_op_divw,
  power_op_creqv,
  power_op_fctiwz,
  power_op_crnor,
  power_op_lbzux,
  power_op_td,
  power_op_dcbi,
  power_op_cli,
  power_op_div,
  power_op_add,
  power_op_extsh,
  power_op_divd,
  power_op_fmsub,
  power_op_stbx,
  power_op_nabs,
  power_op_isync,
  power_op_mfsri,
  power_op_stfdx,
  power_op_fsqrt,
  power_op_dcbz,
  power_op_dcbst,
  power_op_stswi,
  power_op_mulli,
  power_op_stfs,
  power_op_clf,
  power_op_fnmsub,
  power_op_lhz,
  power_op_ecowx,
  power_op_fres,
  power_op_stwu,
  power_op_lhau,
  power_op_slq,
  power_op_srawi,
  power_op_divwu,
  power_op_addis,
  power_op_mfmsr,
  power_op_mulhd,
  power_op_fdivs,
  power_op_abs,
  power_op_lwzu,
  power_op_tlbli,
  power_op_orc,
  power_op_mtfsf,
  power_op_lswx,
  power_op_stb,
  power_op_andis_rc,
  power_op_fsel,
  power_op_xori,
  power_op_lwax,
  power_op_tdi,
  power_op_rlwimi,
  power_op_stw,
  power_op_rldcr,
  power_op_sraq,
  power_op_fmr,
  power_op_tlbld,
  power_op_doz,
  power_op_lbz,
  power_op_stdux,
  power_op_mtfsfi,
  power_op_srea,
  power_op_lscbx,
  power_op_rlwinm,
  power_op_sld,
  power_op_addc,
  power_op_lfqux,
  power_op_sleq,
  power_op_extsb,
  power_op_ld,
  power_op_ldu,
  power_op_fctidz,
  power_op_lfq,
  power_op_lwbrx,
  power_op_fsqrts,
  power_op_srd,
  power_op_lfdu,
  power_op_stfsux,
  power_op_lhzu,
  power_op_crnand,
  power_op_icbi,
  power_op_rlwnm,
  power_op_rldcl,
  power_op_stwcx_rc,
  power_op_lhzx,
  power_op_stfsx,
  power_op_rlmi,
  power_op_twi,
  power_op_srliq,
  power_op_tlbie,
  power_op_mfcr,
  power_op_tlbsync,
  power_op_extsw,
  power_op_rldicl,
  power_op_bclr,
  power_op_rfsvc,
  power_op_mcrxr,
  power_op_clcs,
  power_op_srad,
  power_op_subfc,
  power_op_mfsrin,
  power_op_rfi,
  power_op_sreq,
  power_op_frsqrte,
  power_op_mffs,
  power_op_lwz,
  power_op_lfqu,
  power_op_and,
  power_op_stswx,
  power_op_stfd,
  power_op_fmsubs,
  power_op_bcctr,
  power_op_lhaux,
  power_op_ldux,
  power_op_fctid,
  power_op_frsp,
  power_op_slw,
  power_op_cmpli,
  power_op_sync,
  power_op_cntlzw,
  power_op_maskg,
  power_op_divdu,
  power_op_xor,
  power_op_fadds,
  power_op_fneg,
  power_op_lwaux,
  power_op_fsub,
  power_op_stfqux,
  power_op_srlq,
  power_op_lfqx,
  power_op_dcbt,
  power_op_sliq,
  power_op_fcmpo,
  power_op_lhax,
  power_op_cror,
  power_op_dozi,
  power_op_crand,
  power_op_stfsu,
  power_op_lha,
  power_op_mcrf,
  power_op_fdiv,
  power_op_ori,
  power_op_fmadd,
  power_op_stmw,
  power_op_lwarx,
  power_op_sle,
  power_op_fsubs,
  power_op_stdx,
  power_op_stwx,
  power_op_sthux,
  power_op_stwbrx,
  power_op_sthu,
  power_op_dclst,
  power_op_fcmpu,
  power_op_subfme,
  power_op_stfiwx,
  power_op_mul,
  power_op_bc,
  power_op_stwux,
  power_op_sllq,
  power_op_mullw,
  power_op_cmpi,
  power_op_rldicr,
  power_op_sth,
  power_op_sre,
  power_op_slliq,
  power_op_rldic,
  power_op_fnabs,
  power_op_sc,
  power_op_addic_rc,
  power_op_rldimi,
  power_op_stfqu,
  power_op_neg,
  power_op_oris,
  power_op_lfsux,
  power_op_mtfsb1,
  power_op_dcbtst,
  power_op_subfe,
  power_op_b,
  power_op_lwzux,
  power_op_rac,
  power_op_lfdux,
  power_op_lbzu,
  power_op_lhzux,
  power_op_lhbrx,
  power_op_lfsu,
  power_op_srw,
  power_op_crxor,
  power_op_stfdux,
  power_op_lmw,
  power_op_adde,
  power_op_mfsr,
  power_op_sraiq,
  power_op_rrib,
  power_op_addi,
  power_op_sthx,
  power_op_stfqx,
  power_op_andi_rc,
  power_op_or,
  power_op_dcbf,
  power_op_fcfid,
  power_op_fmadds,
  power_op_mtfsb0,
  power_op_lswi,
  power_op_mulhdu,
  power_op_ldarx,
  power_op_eieio,
  power_op_cntlzd,
  power_op_subfze,
  power_op_fabs,
  power_op_tw,
  power_op_eqv,
  power_op_stfq,
  power_op_maskir,
  power_op_sriq,
  power_op_mfspr,
  power_op_ldx,
  power_op_crorc,
  power_op_lfd,
  power_op_cmp,
  power_op_stbu,
  power_op_stfpdux,
  power_op_stfpdx,
  power_op_stfpsux,
  power_op_stfpsx,
  power_op_stfxdux,
  power_op_stfxdx,
  power_op_stfxsux,
  power_op_stfxsx,
  power_op_stfsdux,
  power_op_stfsdx,
  power_op_stfssux,
  power_op_stfssx,
  power_op_stfpiwx,
  power_op_lfpdux,
  power_op_lfpdx,
  power_op_lfpsux,
  power_op_lfpsx,
  power_op_lfxdux,
  power_op_lfxdx,
  power_op_lfxsux,
  power_op_lfxsx,
  power_op_lfsdux,
  power_op_lfsdx,
  power_op_lfssux,
  power_op_lfssx,
  power_op_fxcxnms,
  power_op_fxcxma,
  power_op_fxcxnsma,
  power_op_fxcxnpma,
  power_op_fxcsnsma,
  power_op_fxcpnsma,
  power_op_fxcsnpma,
  power_op_fxcpnpma,
  power_op_fsmtp,
  power_op_fsmfp,
  power_op_fpctiwz,
  power_op_fpctiw,
  power_op_fxmr,
  power_op_fpsel,
  power_op_fpmul,
  power_op_fxmul,
  power_op_fxpmul,
  power_op_fxsmul,
  power_op_fpadd,
  power_op_fpsub,
  power_op_fpre,
  power_op_fprsqrte,
  power_op_fpmadd,
  power_op_fxmadd,
  power_op_fxcpmadd,
  power_op_fxcsmadd,
  power_op_fpnmadd,
  power_op_fxnmadd,
  power_op_fxcpnmadd,
  power_op_fxcsnmadd,
  power_op_fpmsub,
  power_op_fxmsub,
  power_op_fxcpmsub,
  power_op_fxcsmsub,
  power_op_fpnmsub,
  power_op_fxnmsub,
  power_op_fxcpnmsub,
  power_op_fxcsnmsub,
  power_op_fpmr,
  power_op_fpabs,
  power_op_fpneg,
  power_op_fprsp,
  power_op_fpnabs,
  power_op_fsmr,
  power_op_fscmp,
  power_op_fsabs,
  power_op_fsneg,
  power_op_fsnabs,
  power_op_lwa,
  _entry_ids_max_
};

enum prefixEntryID {
  prefix_none,
  prefix_rep,
  prefix_repnz
};

#if defined(__GNUC__)
//***************** GCC ***********************
  #if !defined(cap_tr1)
  //**************** GCC < 4.3.0 ************
  namespace __gnu_cxx {
    
    template<> struct hash<entryID> {
      hash<unsigned int> h;
      unsigned operator()(const entryID &e) const 
      {
	return h(static_cast<unsigned int>(e));
      };
    };
    template<> struct hash<prefixEntryID> {
      hash<unsigned int> h;
      unsigned operator()(const prefixEntryID &e) const 
      {
	return h(static_cast<unsigned int>(e));
      };
    };
  }
	#else
  namespace std
  {
    namespace tr1
    {
      template <>
      struct hash<entryID>
      {
        hash<size_t> h;
	size_t operator()(const entryID &eid) const
	{
	  return h(static_cast<size_t>(eid));
	}
      };
      template <>
      struct hash<prefixEntryID>
      {
        hash<size_t> h;
	size_t operator()(const prefixEntryID &eid) const
	{
	  return h(static_cast<size_t>(eid));
	}
      };
    }
  }
	#endif
#endif
namespace NS_x86 {
COMMON_EXPORT extern dyn_hash_map<entryID, std::string> entryNames_IAPI;
COMMON_EXPORT extern dyn_hash_map<prefixEntryID, std::string> prefixEntryNames_IAPI;
}

#endif // defined(ENTRYIDS_IA32_H)
