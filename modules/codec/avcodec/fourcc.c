/*****************************************************************************
 * fourcc.c: libavcodec <-> libvlc conversion routines
 *****************************************************************************
 * Copyright (C) 1999-2009 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Laurent Aimar <fenrir@via.ecp.fr>
 *          Gildas Bazin <gbazin@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <vlc_common.h>
#include <vlc_codec.h>

#include <libavcodec/avcodec.h>
#include "avcodec.h"

/*****************************************************************************
 * Codec fourcc -> libavcodec Codec_id mapping
 * Sorted by AVCodecID enumeration order
 *****************************************************************************/
static const struct
{
    vlc_fourcc_t i_fourcc;
    unsigned i_codec;
    int i_cat;
} codecs_table[] =
{
    /*
     * Video Codecs
     */

    { VLC_CODEC_MP2V, AV_CODEC_ID_MPEG2VIDEO, VIDEO_ES }, /* prefer MPEG2 over MPEG1 */
    { VLC_CODEC_MPGV, AV_CODEC_ID_MPEG2VIDEO, VIDEO_ES }, /* prefer MPEG2 over MPEG1 */
    { VLC_CODEC_MP1V, AV_CODEC_ID_MPEG1VIDEO, VIDEO_ES },
    /* AV_CODEC_ID_MPEG2VIDEO_XVMC */
    { VLC_CODEC_H261, AV_CODEC_ID_H261, VIDEO_ES },
    { VLC_CODEC_H263, AV_CODEC_ID_H263, VIDEO_ES },
    { VLC_CODEC_RV10, AV_CODEC_ID_RV10, VIDEO_ES },
    { VLC_CODEC_RV13, AV_CODEC_ID_RV10, VIDEO_ES },
    { VLC_CODEC_RV20, AV_CODEC_ID_RV20, VIDEO_ES },
    { VLC_CODEC_MJPG, AV_CODEC_ID_MJPEG, VIDEO_ES },
    { VLC_CODEC_MJPGB, AV_CODEC_ID_MJPEGB,VIDEO_ES },
    { VLC_CODEC_LJPG, AV_CODEC_ID_LJPEG, VIDEO_ES },
    { VLC_CODEC_SP5X, AV_CODEC_ID_SP5X, VIDEO_ES },
    { VLC_CODEC_JPEGLS, AV_CODEC_ID_JPEGLS, VIDEO_ES },
    { VLC_CODEC_MP4V, AV_CODEC_ID_MPEG4, VIDEO_ES },
    /* AV_CODEC_ID_RAWVIDEO */
    { VLC_CODEC_DIV1, AV_CODEC_ID_MSMPEG4V1, VIDEO_ES },
    { VLC_CODEC_DIV2, AV_CODEC_ID_MSMPEG4V2, VIDEO_ES },
    { VLC_CODEC_DIV3, AV_CODEC_ID_MSMPEG4V3, VIDEO_ES },
    { VLC_CODEC_WMV1, AV_CODEC_ID_WMV1, VIDEO_ES },
    { VLC_CODEC_WMV2, AV_CODEC_ID_WMV2, VIDEO_ES },
    { VLC_CODEC_H263P, AV_CODEC_ID_H263P, VIDEO_ES },
    { VLC_CODEC_H263I, AV_CODEC_ID_H263I, VIDEO_ES },
    { VLC_CODEC_FLV1, AV_CODEC_ID_FLV1, VIDEO_ES },
    { VLC_CODEC_SVQ1, AV_CODEC_ID_SVQ1, VIDEO_ES },
    { VLC_CODEC_SVQ3, AV_CODEC_ID_SVQ3, VIDEO_ES },
    { VLC_CODEC_DV, AV_CODEC_ID_DVVIDEO, VIDEO_ES },
    { VLC_CODEC_HUFFYUV, AV_CODEC_ID_HUFFYUV, VIDEO_ES },
    { VLC_CODEC_CYUV, AV_CODEC_ID_CYUV, VIDEO_ES },
    { VLC_CODEC_H264, AV_CODEC_ID_H264, VIDEO_ES },
    { VLC_CODEC_INDEO3, AV_CODEC_ID_INDEO3, VIDEO_ES },
    { VLC_CODEC_VP3, AV_CODEC_ID_VP3, VIDEO_ES },
    { VLC_CODEC_THEORA, AV_CODEC_ID_THEORA, VIDEO_ES },
#if ( !defined( WORDS_BIGENDIAN ) )
    /* Asus Video (Another thing that doesn't work on PPC) */
    { VLC_CODEC_ASV1, AV_CODEC_ID_ASV1, VIDEO_ES },
    { VLC_CODEC_ASV2, AV_CODEC_ID_ASV2, VIDEO_ES },
#endif
    { VLC_CODEC_FFV1, AV_CODEC_ID_FFV1, VIDEO_ES },
    { VLC_CODEC_4XM, AV_CODEC_ID_4XM, VIDEO_ES },
    { VLC_CODEC_VCR1, AV_CODEC_ID_VCR1, VIDEO_ES },
    { VLC_CODEC_CLJR, AV_CODEC_ID_CLJR, VIDEO_ES },
    { VLC_CODEC_MDEC, AV_CODEC_ID_MDEC, VIDEO_ES },
    { VLC_CODEC_ROQ, AV_CODEC_ID_ROQ, VIDEO_ES },
    { VLC_CODEC_INTERPLAY, AV_CODEC_ID_INTERPLAY_VIDEO, VIDEO_ES },
    { VLC_CODEC_XAN_WC3, AV_CODEC_ID_XAN_WC3, VIDEO_ES },
    { VLC_CODEC_XAN_WC4, AV_CODEC_ID_XAN_WC4, VIDEO_ES },
    { VLC_CODEC_RPZA, AV_CODEC_ID_RPZA, VIDEO_ES },
    { VLC_CODEC_CINEPAK, AV_CODEC_ID_CINEPAK, VIDEO_ES },
    { VLC_CODEC_WS_VQA, AV_CODEC_ID_WS_VQA, VIDEO_ES },
    { VLC_CODEC_MSRLE, AV_CODEC_ID_MSRLE, VIDEO_ES },
    { VLC_CODEC_MSVIDEO1, AV_CODEC_ID_MSVIDEO1, VIDEO_ES },
    { VLC_CODEC_IDCIN, AV_CODEC_ID_IDCIN, VIDEO_ES },
    { VLC_CODEC_8BPS, AV_CODEC_ID_8BPS, VIDEO_ES },
    { VLC_CODEC_SMC, AV_CODEC_ID_SMC, VIDEO_ES },
    { VLC_CODEC_FLIC, AV_CODEC_ID_FLIC, VIDEO_ES },
    { VLC_CODEC_TRUEMOTION1, AV_CODEC_ID_TRUEMOTION1, VIDEO_ES },
    { VLC_CODEC_VMDVIDEO, AV_CODEC_ID_VMDVIDEO, VIDEO_ES },
    { VLC_CODEC_LCL_MSZH, AV_CODEC_ID_MSZH, VIDEO_ES },
    { VLC_CODEC_LCL_ZLIB, AV_CODEC_ID_ZLIB, VIDEO_ES },
    { VLC_CODEC_QTRLE, AV_CODEC_ID_QTRLE, VIDEO_ES },
    { VLC_CODEC_TSCC, AV_CODEC_ID_TSCC, VIDEO_ES },
    { VLC_CODEC_ULTI, AV_CODEC_ID_ULTI, VIDEO_ES },
    { VLC_CODEC_QDRAW, AV_CODEC_ID_QDRAW, VIDEO_ES },
    { VLC_CODEC_VIXL, AV_CODEC_ID_VIXL, VIDEO_ES },
    { VLC_CODEC_QPEG, AV_CODEC_ID_QPEG, VIDEO_ES },
    { VLC_CODEC_PNG, AV_CODEC_ID_PNG, VIDEO_ES },
    { VLC_CODEC_PPM, AV_CODEC_ID_PPM, VIDEO_ES },
    /* AV_CODEC_ID_PBM */
    { VLC_CODEC_PGM, AV_CODEC_ID_PGM, VIDEO_ES },
    { VLC_CODEC_PGMYUV, AV_CODEC_ID_PGMYUV, VIDEO_ES },
    { VLC_CODEC_PAM, AV_CODEC_ID_PAM, VIDEO_ES },
    { VLC_CODEC_FFVHUFF, AV_CODEC_ID_FFVHUFF, VIDEO_ES },
    { VLC_CODEC_RV30, AV_CODEC_ID_RV30, VIDEO_ES },
    { VLC_CODEC_RV40, AV_CODEC_ID_RV40, VIDEO_ES },
    { VLC_CODEC_VC1,  AV_CODEC_ID_VC1, VIDEO_ES },
    { VLC_CODEC_WMVA, AV_CODEC_ID_VC1, VIDEO_ES },
    { VLC_CODEC_WMV3, AV_CODEC_ID_WMV3, VIDEO_ES },
    { VLC_CODEC_WMVP, AV_CODEC_ID_WMV3, VIDEO_ES },
    { VLC_CODEC_LOCO, AV_CODEC_ID_LOCO, VIDEO_ES },
    { VLC_CODEC_WNV1, AV_CODEC_ID_WNV1, VIDEO_ES },
    { VLC_CODEC_AASC, AV_CODEC_ID_AASC, VIDEO_ES },
    { VLC_CODEC_INDEO2, AV_CODEC_ID_INDEO2, VIDEO_ES },
    { VLC_CODEC_FRAPS, AV_CODEC_ID_FRAPS, VIDEO_ES },
    { VLC_CODEC_TRUEMOTION2, AV_CODEC_ID_TRUEMOTION2, VIDEO_ES },
    { VLC_CODEC_BMP, AV_CODEC_ID_BMP, VIDEO_ES },
    { VLC_CODEC_CSCD, AV_CODEC_ID_CSCD, VIDEO_ES },
    { VLC_CODEC_MMVIDEO, AV_CODEC_ID_MMVIDEO, VIDEO_ES },
    { VLC_CODEC_ZMBV, AV_CODEC_ID_ZMBV, VIDEO_ES },
    { VLC_CODEC_AVS, AV_CODEC_ID_AVS, VIDEO_ES },
    { VLC_CODEC_SMACKVIDEO, AV_CODEC_ID_SMACKVIDEO, VIDEO_ES },
    { VLC_CODEC_NUV, AV_CODEC_ID_NUV, VIDEO_ES },
    { VLC_CODEC_KMVC, AV_CODEC_ID_KMVC, VIDEO_ES },
    { VLC_CODEC_FLASHSV, AV_CODEC_ID_FLASHSV, VIDEO_ES },
    { VLC_CODEC_CAVS, AV_CODEC_ID_CAVS, VIDEO_ES },
    { VLC_CODEC_JPEG2000, AV_CODEC_ID_JPEG2000, VIDEO_ES },
    { VLC_CODEC_VMNC, AV_CODEC_ID_VMNC, VIDEO_ES },
    { VLC_CODEC_VP5, AV_CODEC_ID_VP5, VIDEO_ES },
    { VLC_CODEC_VP6, AV_CODEC_ID_VP6, VIDEO_ES },
    { VLC_CODEC_VP6F, AV_CODEC_ID_VP6F, VIDEO_ES },
    { VLC_CODEC_TARGA, AV_CODEC_ID_TARGA, VIDEO_ES },
    { VLC_CODEC_DSICINVIDEO, AV_CODEC_ID_DSICINVIDEO, VIDEO_ES },
    { VLC_CODEC_TIERTEXSEQVIDEO, AV_CODEC_ID_TIERTEXSEQVIDEO, VIDEO_ES },
    { VLC_CODEC_TIFF, AV_CODEC_ID_TIFF, VIDEO_ES },
    { VLC_CODEC_GIF, AV_CODEC_ID_GIF, VIDEO_ES },
    { VLC_CODEC_DXA, AV_CODEC_ID_DXA, VIDEO_ES },
    { VLC_CODEC_DNXHD, AV_CODEC_ID_DNXHD, VIDEO_ES },
    { VLC_CODEC_THP, AV_CODEC_ID_THP, VIDEO_ES },
    { VLC_CODEC_SGI, AV_CODEC_ID_SGI, VIDEO_ES },
    { VLC_CODEC_C93, AV_CODEC_ID_C93, VIDEO_ES },
    { VLC_CODEC_BETHSOFTVID, AV_CODEC_ID_BETHSOFTVID, VIDEO_ES },
    /* AV_CODEC_ID_PTX */
    { VLC_CODEC_TXD, AV_CODEC_ID_TXD, VIDEO_ES },
    { VLC_CODEC_VP6A, AV_CODEC_ID_VP6A, VIDEO_ES },
    { VLC_CODEC_AMV, AV_CODEC_ID_AMV, VIDEO_ES },
    { VLC_CODEC_VB, AV_CODEC_ID_VB, VIDEO_ES },
    { VLC_CODEC_PCX, AV_CODEC_ID_PCX, VIDEO_ES },
    /* AV_CODEC_ID_SUNRAST */
    { VLC_CODEC_INDEO4, AV_CODEC_ID_INDEO4, VIDEO_ES },
    { VLC_CODEC_INDEO5, AV_CODEC_ID_INDEO5, VIDEO_ES },
    { VLC_CODEC_MIMIC, AV_CODEC_ID_MIMIC, VIDEO_ES },
    { VLC_CODEC_RL2, AV_CODEC_ID_RL2, VIDEO_ES },
    { VLC_CODEC_ESCAPE124, AV_CODEC_ID_ESCAPE124, VIDEO_ES },
    { VLC_CODEC_DIRAC, AV_CODEC_ID_DIRAC, VIDEO_ES },
    { VLC_CODEC_BFI, AV_CODEC_ID_BFI, VIDEO_ES },
    { VLC_CODEC_CMV, AV_CODEC_ID_CMV, VIDEO_ES },
    /* AV_CODEC_ID_MOTIONPIXELS */
    /* AV_CODEC_ID_TGV */
    /* AV_CODEC_ID_TGQ */
    /* AV_CODEC_ID_TQI */
    { VLC_CODEC_AURA, AV_CODEC_ID_AURA, VIDEO_ES },
    /* AV_CODEC_ID_AURA2 */
    /* AV_CODEC_ID_V210X */
    { VLC_CODEC_TMV, AV_CODEC_ID_TMV, VIDEO_ES },
    { VLC_CODEC_V210, AV_CODEC_ID_V210, VIDEO_ES },
    /* AV_CODEC_ID_DPX */
    /* AV_CODEC_ID_MAD */
    { VLC_CODEC_FRWU, AV_CODEC_ID_FRWU, VIDEO_ES },
    { VLC_CODEC_FLASHSV2, AV_CODEC_ID_FLASHSV2, VIDEO_ES },
    /* AV_CODEC_ID_CDGRAPHICS */
    /* AV_CODEC_ID_R210 */
    /* AV_CODEC_ID_ANM */
    { VLC_CODEC_BINKVIDEO, AV_CODEC_ID_BINKVIDEO, VIDEO_ES },
    /* AV_CODEC_ID_IFF_ILBM */
    /* AV_CODEC_ID_IFF_BYTERUN1 */
    { VLC_CODEC_KGV1, AV_CODEC_ID_KGV1, VIDEO_ES },
    /* AV_CODEC_ID_YOP */
    { VLC_CODEC_VP8, AV_CODEC_ID_VP8, VIDEO_ES },
    /* AV_CODEC_ID_PICTOR */
    /* AV_CODEC_ID_ANSI */
    /* AV_CODEC_ID_A64_MULTI */
    /* AV_CODEC_ID_A64_MULTI5 */
    /* AV_CODEC_ID_R10K */
    { VLC_CODEC_MXPEG, AV_CODEC_ID_MXPEG, VIDEO_ES },
    { VLC_CODEC_LAGARITH, AV_CODEC_ID_LAGARITH, VIDEO_ES },
    { VLC_CODEC_PRORES, AV_CODEC_ID_PRORES, VIDEO_ES },
    /* AV_CODEC_ID_JV */
    /* AV_CODEC_ID_DFA */
    { VLC_CODEC_WMVP, AV_CODEC_ID_WMV3IMAGE, VIDEO_ES },
    { VLC_CODEC_WMVP2, AV_CODEC_ID_VC1IMAGE, VIDEO_ES },
    { VLC_CODEC_UTVIDEO, AV_CODEC_ID_UTVIDEO, VIDEO_ES },
    { VLC_CODEC_BMVVIDEO, AV_CODEC_ID_BMV_VIDEO, VIDEO_ES },
    { VLC_CODEC_VBLE, AV_CODEC_ID_VBLE, VIDEO_ES },
    { VLC_CODEC_DXTORY, AV_CODEC_ID_DXTORY, VIDEO_ES },
    /* AV_CODEC_ID_V410 */
    /* AV_CODEC_ID_XWD */
    { VLC_CODEC_CDXL, AV_CODEC_ID_CDXL, VIDEO_ES },
    /* AV_CODEC_ID_XBM */
    /* AV_CODEC_ID_ZEROCODEC */
#if LIBAVCODEC_VERSION_CHECK( 54, 16, 0, 27, 100 )
    { VLC_CODEC_MSS1, AV_CODEC_ID_MSS1, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 54, 17, 0, 32, 100 )
    { VLC_CODEC_MSA1, AV_CODEC_ID_MSA1, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 54, 18, 0, 34, 100 )
    { VLC_CODEC_TSC2, AV_CODEC_ID_TSCC2, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 54, 20, 0, 37, 100 )
    { VLC_CODEC_MTS2, AV_CODEC_ID_MTS2, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 54, 24, 0, 47, 100 )
    { VLC_CODEC_CLLC, AV_CODEC_ID_CLLC, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 54, 27, 0, 55, 100 )
    { VLC_CODEC_MSS2, AV_CODEC_ID_MSS2, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 54, 41, 0, 89, 100 )
    { VLC_CODEC_VP9, AV_CODEC_ID_VP9, VIDEO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 55, 5, 0, 10, 100 )
    { VLC_CODEC_ICOD, AV_CODEC_ID_AIC, VIDEO_ES },
#endif
    /* AV_CODEC_ID_ESCAPE130 */
#if LIBAVCODEC_VERSION_CHECK( 55, 8, 0, 15, 100 )
    { VLC_CODEC_G2M4, AV_CODEC_ID_G2M, VIDEO_ES },
#endif
    /* AV_CODEC_ID_WEBP */
    /* AV_CODEC_ID_HNM4_VIDEO */
#if LIBAVCODEC_VERSION_CHECK( 55, 24, 0, 37, 100 )
    { VLC_CODEC_HEVC, AV_CODEC_ID_HEVC, VIDEO_ES },
#endif

#if LIBAVCODEC_VERSION_CHECK( 55, 33, 0, 50, 100 )
    { VLC_CODEC_FIC , AV_CODEC_ID_FIC, VIDEO_ES },
#endif

#if LIBAVCODEC_VERSION_CHECK( 55, 43, 0, 56, 108 )
    { VLC_CODEC_VP7 , AV_CODEC_ID_VP7, VIDEO_ES },
#endif


    /* ffmpeg only: AV_CODEC_ID_BRENDER_PIX */
    /* ffmpeg only: AV_CODEC_ID_Y41P */
    /* ffmpeg only: AV_CODEC_ID_EXR */
    /* ffmpeg only: AV_CODEC_ID_AVRP */
    /* ffmpeg only: AV_CODEC_ID_012V */
    /* ffmpeg only: AV_CODEC_ID_AVUI */
    /* ffmpeg only: AV_CODEC_ID_AYUV */
    /* ffmpeg only: AV_CODEC_ID_TARGA_Y216 */
    /* ffmpeg only: AV_CODEC_ID_V308 */
    /* ffmpeg only: AV_CODEC_ID_V408 */
    /* ffmpeg only: AV_CODEC_ID_YUV4 */
    /* ffmpeg only: AV_CODEC_ID_SANM */
    /* ffmpeg only: AV_CODEC_ID_PAF_VIDEO */
    /* ffmpeg only: AV_CODEC_ID_AVRN */
    /* ffmpeg only: AV_CODEC_ID_CPIA */
    /* ffmpeg only: AV_CODEC_ID_XFACE */
    /* ffmpeg only: AV_CODEC_ID_SGIRLE */
    /* ffmpeg only: AV_CODEC_ID_MVC1 */
    /* ffmpeg only: AV_CODEC_ID_MVC2 */
    /* ffmpeg only: AV_CODEC_ID_SNOW */
    /* ffmpeg only: AV_CODEC_ID_SMVJPEG */


    /*
     *  Audio Codecs
     */

    /* PCM */
    { VLC_CODEC_S16L, AV_CODEC_ID_PCM_S16LE, AUDIO_ES },
    { VLC_CODEC_S16B, AV_CODEC_ID_PCM_S16BE, AUDIO_ES },
    { VLC_CODEC_U16L, AV_CODEC_ID_PCM_U16LE, AUDIO_ES },
    { VLC_CODEC_U16B, AV_CODEC_ID_PCM_U16BE, AUDIO_ES },
    { VLC_CODEC_S8, AV_CODEC_ID_PCM_S8, AUDIO_ES },
    { VLC_CODEC_U8, AV_CODEC_ID_PCM_U8, AUDIO_ES },
    { VLC_CODEC_MULAW, AV_CODEC_ID_PCM_MULAW, AUDIO_ES },
    { VLC_CODEC_ALAW, AV_CODEC_ID_PCM_ALAW, AUDIO_ES },
    { VLC_CODEC_S32L, AV_CODEC_ID_PCM_S32LE, AUDIO_ES },
    { VLC_CODEC_S32B, AV_CODEC_ID_PCM_S32BE, AUDIO_ES },
    { VLC_CODEC_U32L, AV_CODEC_ID_PCM_U32LE, AUDIO_ES },
    { VLC_CODEC_U32B, AV_CODEC_ID_PCM_U32BE, AUDIO_ES },
    { VLC_CODEC_S24L, AV_CODEC_ID_PCM_S24LE, AUDIO_ES },
    { VLC_CODEC_S24B, AV_CODEC_ID_PCM_S24BE, AUDIO_ES },
    { VLC_CODEC_U24L, AV_CODEC_ID_PCM_U24LE, AUDIO_ES },
    { VLC_CODEC_U24B, AV_CODEC_ID_PCM_U24BE, AUDIO_ES },
    { VLC_CODEC_S24DAUD, AV_CODEC_ID_PCM_S24DAUD, AUDIO_ES },
    /* AV_CODEC_ID_PCM_ZORK */
    /* AV_CODEC_ID_PCM_S16LE_PLANAR */
    /* AV_CODEC_ID_PCM_DVD */
    { VLC_CODEC_F32B, AV_CODEC_ID_PCM_F32BE, AUDIO_ES },
    { VLC_CODEC_F32L, AV_CODEC_ID_PCM_F32LE, AUDIO_ES },
    { VLC_CODEC_F64B, AV_CODEC_ID_PCM_F64BE, AUDIO_ES },
    { VLC_CODEC_F64L, AV_CODEC_ID_PCM_F64LE, AUDIO_ES },
    { VLC_CODEC_BD_LPCM, AV_CODEC_ID_PCM_BLURAY, AUDIO_ES },
    /* AV_CODEC_ID_PCM_LXF */
    /* AV_CODEC_ID_S302M */
    /* AV_CODEC_ID_PCM_S8_PLANAR */
    /* AV_CODEC_ID_PCM_S24LE_PLANAR */
    /* AV_CODEC_ID_PCM_S32LE_PLANAR */
    /* ffmpeg only: AV_CODEC_ID_PCM_S16BE_PLANAR */

    /* ADPCM */
    { VLC_CODEC_ADPCM_IMA_QT, AV_CODEC_ID_ADPCM_IMA_QT, AUDIO_ES },
    { VLC_CODEC_ADPCM_IMA_WAV, AV_CODEC_ID_ADPCM_IMA_WAV, AUDIO_ES },
    /* AV_CODEC_ID_ADPCM_IMA_DK3 */
    /* AV_CODEC_ID_ADPCM_IMA_DK4 */
    { VLC_CODEC_ADPCM_IMA_WS, AV_CODEC_ID_ADPCM_IMA_WS, AUDIO_ES },
    /* AV_CODEC_ID_ADPCM_IMA_SMJPEG */
    { VLC_CODEC_ADPCM_MS, AV_CODEC_ID_ADPCM_MS, AUDIO_ES },
    { VLC_CODEC_ADPCM_4XM, AV_CODEC_ID_ADPCM_4XM, AUDIO_ES },
    { VLC_CODEC_ADPCM_XA, AV_CODEC_ID_ADPCM_XA, AUDIO_ES },
    { VLC_CODEC_ADPCM_ADX, AV_CODEC_ID_ADPCM_ADX, AUDIO_ES },
    { VLC_CODEC_ADPCM_EA, AV_CODEC_ID_ADPCM_EA, AUDIO_ES },
    { VLC_CODEC_ADPCM_G726, AV_CODEC_ID_ADPCM_G726, AUDIO_ES },
    /* AV_CODEC_ID_ADPCM_CT */
    { VLC_CODEC_ADPCM_SWF, AV_CODEC_ID_ADPCM_SWF, AUDIO_ES },
    { VLC_CODEC_ADPCM_YAMAHA, AV_CODEC_ID_ADPCM_YAMAHA, AUDIO_ES },
    /* AV_CODEC_ID_ADPCM_SBPRO_4 */
    /* AV_CODEC_ID_ADPCM_SBPRO_3 */
    /* AV_CODEC_ID_ADPCM_SBPRO_2 */
    { VLC_CODEC_ADPCM_THP, AV_CODEC_ID_ADPCM_THP, AUDIO_ES },
    { VLC_CODEC_ADPCM_IMA_AMV, AV_CODEC_ID_ADPCM_IMA_AMV, AUDIO_ES },
    /* AV_CODEC_ID_ADPCM_EA_R1 */
    /* AV_CODEC_ID_ADPCM_EA_R3 */
    /* AV_CODEC_ID_ADPCM_EA_R2 */
    /* AV_CODEC_ID_ADPCM_IMA_EA_SEAD */
    /* AV_CODEC_ID_ADPCM_IMA_EA_EACS */
    /* AV_CODEC_ID_ADPCM_EA_XAS */
    /* AV_CODEC_ID_ADPCM_EA_MAXIS_XA */
    /* AV_CODEC_ID_ADPCM_IMA_ISS */
    { VLC_CODEC_ADPCM_G722, AV_CODEC_ID_ADPCM_G722, AUDIO_ES },
    /* AV_CODEC_ID_ADPCM_IMA_APC */
    /* ffmpeg only: AV_CODEC_ID_VIMA */
    /* ffmpeg only: AV_CODEC_ID_ADPCM_AFC */
    /* ffmpeg only: AV_CODEC_ID_ADPCM_IMA_OKI */
    /* ffmpeg only: AV_CODEC_ID_ADPCM_DTK */
    /* ffmpeg only: AV_CODEC_ID_ADPCM_IMA_RAD */
    /* ffmpeg only: AV_CODEC_ID_ADPCM_G726LE */

    /* AMR */
    { VLC_CODEC_AMR_NB, AV_CODEC_ID_AMR_NB, AUDIO_ES },
    { VLC_CODEC_AMR_WB, AV_CODEC_ID_AMR_WB, AUDIO_ES },

    /* RealAudio */
    { VLC_CODEC_RA_144, AV_CODEC_ID_RA_144, AUDIO_ES },
    { VLC_CODEC_RA_288, AV_CODEC_ID_RA_288, AUDIO_ES },

    /* DPCM */
    { VLC_CODEC_ROQ_DPCM, AV_CODEC_ID_ROQ_DPCM, AUDIO_ES },
    { VLC_CODEC_INTERPLAY_DPCM, AV_CODEC_ID_INTERPLAY_DPCM, AUDIO_ES },
    /* AV_CODEC_ID_XAN_DPCM */
    /* AV_CODEC_ID_SOL_DPCM */

    /* audio codecs */
    { VLC_CODEC_MP3, AV_CODEC_ID_MP3, AUDIO_ES },
    { VLC_CODEC_MPGA, AV_CODEC_ID_MP2, AUDIO_ES },
    { VLC_CODEC_MP2, AV_CODEC_ID_MP2, AUDIO_ES },
    { VLC_CODEC_MP4A, AV_CODEC_ID_AAC, AUDIO_ES },
    { VLC_CODEC_A52, AV_CODEC_ID_AC3, AUDIO_ES },
    { VLC_CODEC_DTS, AV_CODEC_ID_DTS, AUDIO_ES },
    { VLC_CODEC_VORBIS, AV_CODEC_ID_VORBIS, AUDIO_ES },
    { VLC_CODEC_DVAUDIO, AV_CODEC_ID_DVAUDIO, AUDIO_ES },
    { VLC_CODEC_WMA1, AV_CODEC_ID_WMAV1, AUDIO_ES },
    { VLC_CODEC_WMA2, AV_CODEC_ID_WMAV2, AUDIO_ES },
    { VLC_CODEC_MACE3, AV_CODEC_ID_MACE3, AUDIO_ES },
    { VLC_CODEC_MACE6, AV_CODEC_ID_MACE6, AUDIO_ES },
    { VLC_CODEC_VMDAUDIO, AV_CODEC_ID_VMDAUDIO, AUDIO_ES },
    { VLC_CODEC_FLAC, AV_CODEC_ID_FLAC, AUDIO_ES },
    /* AV_CODEC_ID_MP3ADU */
    /* AV_CODEC_ID_MP3ON4 */
    { VLC_CODEC_SHORTEN, AV_CODEC_ID_SHORTEN, AUDIO_ES },
    { VLC_CODEC_ALAC, AV_CODEC_ID_ALAC, AUDIO_ES },
    /* AV_CODEC_ID_WESTWOOD_SND1 */
    { VLC_CODEC_GSM, AV_CODEC_ID_GSM, AUDIO_ES },
    { VLC_CODEC_QDM2, AV_CODEC_ID_QDM2, AUDIO_ES },
    { VLC_CODEC_COOK, AV_CODEC_ID_COOK, AUDIO_ES },
    { VLC_CODEC_TRUESPEECH, AV_CODEC_ID_TRUESPEECH, AUDIO_ES },
    { VLC_CODEC_TTA, AV_CODEC_ID_TTA, AUDIO_ES },
    { VLC_CODEC_SMACKAUDIO, AV_CODEC_ID_SMACKAUDIO, AUDIO_ES },
    { VLC_CODEC_QCELP, AV_CODEC_ID_QCELP, AUDIO_ES },
    { VLC_CODEC_WAVPACK, AV_CODEC_ID_WAVPACK, AUDIO_ES },
    { VLC_CODEC_DSICINAUDIO, AV_CODEC_ID_DSICINAUDIO, AUDIO_ES },
    { VLC_CODEC_IMC, AV_CODEC_ID_IMC, AUDIO_ES },
    { VLC_CODEC_MUSEPACK7, AV_CODEC_ID_MUSEPACK7, AUDIO_ES },
    { VLC_CODEC_MLP, AV_CODEC_ID_MLP, AUDIO_ES },
    { VLC_CODEC_GSM_MS, AV_CODEC_ID_GSM_MS, AUDIO_ES },
    { VLC_CODEC_ATRAC3, AV_CODEC_ID_ATRAC3, AUDIO_ES },
    { VLC_CODEC_APE, AV_CODEC_ID_APE, AUDIO_ES },
    { VLC_CODEC_NELLYMOSER, AV_CODEC_ID_NELLYMOSER, AUDIO_ES },
    { VLC_CODEC_MUSEPACK8, AV_CODEC_ID_MUSEPACK8, AUDIO_ES },
    { VLC_CODEC_SPEEX, AV_CODEC_ID_SPEEX, AUDIO_ES },
    { VLC_CODEC_WMAS, AV_CODEC_ID_WMAVOICE, AUDIO_ES },
    { VLC_CODEC_WMAP, AV_CODEC_ID_WMAPRO, AUDIO_ES },
    { VLC_CODEC_WMAL, AV_CODEC_ID_WMALOSSLESS, AUDIO_ES },
#if LIBAVCODEC_VERSION_CHECK( 55, 32, 0, 47, 100 )
    { VLC_CODEC_ATRAC3P, AV_CODEC_ID_ATRAC3P, AUDIO_ES },
#endif
    { VLC_CODEC_EAC3, AV_CODEC_ID_EAC3, AUDIO_ES },
    { VLC_CODEC_SIPR, AV_CODEC_ID_SIPR, AUDIO_ES },
    /* AV_CODEC_ID_MP1 */
    { VLC_CODEC_TWINVQ, AV_CODEC_ID_TWINVQ, AUDIO_ES },
    { VLC_CODEC_TRUEHD, AV_CODEC_ID_TRUEHD, AUDIO_ES },
    { VLC_CODEC_ALS, AV_CODEC_ID_MP4ALS, AUDIO_ES },
    { VLC_CODEC_ATRAC1, AV_CODEC_ID_ATRAC1, AUDIO_ES },
    { VLC_CODEC_BINKAUDIO_DCT, AV_CODEC_ID_BINKAUDIO_DCT, AUDIO_ES },
    { VLC_CODEC_BINKAUDIO_RDFT, AV_CODEC_ID_BINKAUDIO_RDFT, AUDIO_ES },
    { VLC_CODEC_MP4A, AV_CODEC_ID_AAC_LATM, AUDIO_ES },
    /* AV_CODEC_ID_QDMC */
    /* AV_CODEC_ID_CELT */
    { VLC_CODEC_G723_1, AV_CODEC_ID_G723_1, AUDIO_ES },
    /* AV_CODEC_ID_G729 */
    /* AV_CODEC_ID_8SVX_EXP */
    /* AV_CODEC_ID_8SVX_FIB */
    { VLC_CODEC_BMVAUDIO, AV_CODEC_ID_BMV_AUDIO, AUDIO_ES },
    { VLC_CODEC_RALF, AV_CODEC_ID_RALF, AUDIO_ES },
#if LIBAVCODEC_VERSION_CHECK( 54, 14, 0, 26, 100 )
    { VLC_CODEC_INDEO_AUDIO, AV_CODEC_ID_IAC, AUDIO_ES },
#endif
    /* AV_CODEC_ID_ILBC */
#if LIBAVCODEC_VERSION_CHECK( 54, 29, 0, 17, 101 )
    { VLC_CODEC_OPUS, AV_CODEC_ID_OPUS, AUDIO_ES },
#endif
    /* AV_CODEC_ID_COMFORT_NOISE */
#if LIBAVCODEC_VERSION_CHECK( 54, 34, 0, 65, 100 )
    { VLC_CODEC_TAK, AV_CODEC_ID_TAK, AUDIO_ES },
#endif
#if LIBAVCODEC_VERSION_CHECK( 55, 15, 0, 24, 100 )
    { VLC_CODEC_METASOUND, AV_CODEC_ID_METASOUND, AUDIO_ES },
#endif
    /* AV_CODEC_ID_PAF_AUDIO */
#if LIBAVCODEC_VERSION_CHECK( 55, 49, 0, 59, 100 )
    { VLC_CODEC_ON2AVC, AV_CODEC_ID_ON2AVC, AUDIO_ES },
#endif

    /* ffmpeg only: AV_CODEC_ID_FFWAVESYNTH */
    /* ffmpeg only: AV_CODEC_ID_SONIC */
    /* ffmpeg only: AV_CODEC_ID_SONIC_LS */
    /* ffmpeg only: AV_CODEC_ID_PAF_AUDIO */
    /* ffmpeg only: AV_CODEC_ID_EVRC */
    /* ffmpeg only: AV_CODEC_ID_SMV */

    /* Subtitle streams */
    { VLC_CODEC_SPU, AV_CODEC_ID_DVD_SUBTITLE, SPU_ES },
    { VLC_CODEC_DVBS, AV_CODEC_ID_DVB_SUBTITLE, SPU_ES },
    { VLC_CODEC_SUBT, AV_CODEC_ID_TEXT, SPU_ES },
    { VLC_CODEC_XSUB, AV_CODEC_ID_XSUB, SPU_ES },
    { VLC_CODEC_SSA, AV_CODEC_ID_SSA, SPU_ES },
    /* AV_CODEC_ID_MOV_TEXT */
    { VLC_CODEC_BD_PG, AV_CODEC_ID_HDMV_PGS_SUBTITLE, SPU_ES },
    { VLC_CODEC_TELETEXT, AV_CODEC_ID_DVB_TELETEXT, SPU_ES },
    /* AV_CODEC_ID_SRT */
    /* ffmpeg only: AV_CODEC_ID_MICRODVD */
    /* ffmpeg only: AV_CODEC_ID_EIA_608 */
    /* ffmpeg only: AV_CODEC_ID_JACOSUB */
    /* ffmpeg only: AV_CODEC_ID_SAMI */
    /* ffmpeg only: AV_CODEC_ID_REALTEXT */
    /* ffmpeg only: AV_CODEC_ID_SUBVIEWER1 */
    /* ffmpeg only: AV_CODEC_ID_SUBVIEWER */
    /* ffmpeg only: AV_CODEC_ID_SUBRIP */
    /* ffmpeg only: AV_CODEC_ID_WEBVTT */
    /* ffmpeg only: AV_CODEC_ID_MPL2 */
    /* ffmpeg only: AV_CODEC_ID_VPLAYER */
    /* ffmpeg only: AV_CODEC_ID_PJS */
    /* ffmpeg only: AV_CODEC_ID_ASS */
};

static const size_t codecs_count = sizeof (codecs_table)
                                 / sizeof (codecs_table[0]);

int GetFfmpegCodec( vlc_fourcc_t i_fourcc, int *pi_cat,
                    unsigned *pi_ffmpeg_codec, const char **ppsz_name )
{
    i_fourcc = vlc_fourcc_GetCodec( UNKNOWN_ES, i_fourcc );
    for( unsigned i = 0; i < codecs_count; i++ )
    {
        if( codecs_table[i].i_fourcc == i_fourcc )
        {
            if( pi_cat ) *pi_cat = codecs_table[i].i_cat;
            if( pi_ffmpeg_codec ) *pi_ffmpeg_codec = codecs_table[i].i_codec;
            if( ppsz_name ) *ppsz_name = vlc_fourcc_GetDescription( UNKNOWN_ES, i_fourcc );//char *)codecs_table[i].psz_name;

            return true;
        }
    }
    return false;
}

int GetVlcFourcc( unsigned i_ffmpeg_codec, int *pi_cat,
                  vlc_fourcc_t *pi_fourcc, const char **ppsz_name )
{
    for( unsigned i = 0; i < codecs_count; i++ )
    {
        if( codecs_table[i].i_codec == i_ffmpeg_codec )
        {
            if( pi_cat ) *pi_cat = codecs_table[i].i_cat;
            if( pi_fourcc ) *pi_fourcc = codecs_table[i].i_fourcc;
            if( ppsz_name ) *ppsz_name = vlc_fourcc_GetDescription( codecs_table[i].i_cat, codecs_table[i].i_fourcc );

            return true;
        }
    }
    return false;
}
