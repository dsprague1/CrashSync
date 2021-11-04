/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
CrashSyncAudioProcessor::CrashSyncAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    m_pFrequency = new juce::AudioParameterFloat("frequency", "Frequency", 0.0f, 1.0f, 0.0f);
    m_pThreshold = new juce::AudioParameterFloat("threshold", "Threshold", 0.0f, 1.0f, 0.5f);
    m_pGain = new juce::AudioParameterFloat("gain", "Gain", 0.0f, 1.0f, 0.0f);
    m_pWaveform = new juce::AudioParameterInt("waveform", "Waveform", SCOscillator::kWaveformTri, SCOscillator::numWaveforms - 1, SCOscillator::kWaveformSaw);
    m_pInputMode = new juce::AudioParameterInt("input_mode", "Input Mode", kInputModeNormal, kNumInputMode - 1, kInputModeNormal);
    m_pEnvAttack = new juce::AudioParameterFloat("env_attack", "Env Attack", 0.f, 1.f, 0.1);
    m_pEnvRelease = new juce::AudioParameterFloat("env_release", "Env Release", 0.f, 1.f, 0.1);
    m_pPolyBlep = new juce::AudioParameterInt("polyblep", "PolyBLEP", 0.f, 1.f, 0);
    m_pPulseWidth = new juce::AudioParameterFloat("pulse_width", "Pulse Width", 0.f, 1.f, 0.5);
    m_pOutputVolume = new juce::AudioParameterFloat("output_volume", "Output Volume", 0.f, 1.f, 0.8);
    m_pTone = new juce::AudioParameterFloat("tone", "Tone", 0.f, 1.f, 0.8);

    addParameter(m_pFrequency);
    addParameter(m_pThreshold);
    addParameter(m_pGain);
    addParameter(m_pWaveform);
    addParameter(m_pInputMode);
    addParameter(m_pEnvAttack);
    addParameter(m_pEnvRelease);
    addParameter(m_pPolyBlep);
    addParameter(m_pPulseWidth);
    addParameter(m_pOutputVolume);
    addParameter(m_pTone);

	// now init the units
	m_Interpolator.init(m_nOversamplingRatio, OversamplingIRLength, &m_h_Left[0]);
	m_Decimator.init(m_nOversamplingRatio, OversamplingIRLength, &m_h_Left[0]);

	// dynamically allocate the input x buffers and save the pointers
	m_pLeftInterpBuffer = new float[m_nOversamplingRatio];
	m_pRightInterpBuffer = new float[m_nOversamplingRatio];

	// flush interp buffers
	memset(m_pLeftInterpBuffer, 0, m_nOversamplingRatio * sizeof(float));
	memset(m_pRightInterpBuffer, 0, m_nOversamplingRatio * sizeof(float));

	// dynamically allocate the input x buffers and save the pointers
	m_pLeftDecipBuffer = new float[m_nOversamplingRatio];
	m_pRightDeciBuffer = new float[m_nOversamplingRatio];

	// flush deci buffers
	memset(m_pLeftDecipBuffer, 0, m_nOversamplingRatio * sizeof(float));
	memset(m_pRightDeciBuffer, 0, m_nOversamplingRatio * sizeof(float));

	// Finish initializations here
	m_nOversamplingRatio = 4;

	// NOTE: Both Interpolation and Decimation filters are 
	// Optimal Method Filters designed at 176400Hz; fpass = 20kHz, fstop = 22kHz
	// pb ripple = 0.1 dB
	// sd  < -96dB
	// adjusted till ripple was minimized AND length was a multiple of OS Ratio (4) for polyphase later
	// used RackAFX FIR Designer
	// the Left IR array is used for both filters
	// 4X Coeffs

	m_h_Left[0] = 0.0000005445158422;
	m_h_Left[1] = 0.0000005707121318;
	m_h_Left[2] = -0.0000002019931742;
	m_h_Left[3] = -0.0000027811349810;
	m_h_Left[4] = -0.0000079230067058;
	m_h_Left[5] = -0.0000157045305968;
	m_h_Left[6] = -0.0000250798548223;
	m_h_Left[7] = -0.0000337998826581;
	m_h_Left[8] = -0.0000389039269066;
	m_h_Left[9] = -0.0000377439646400;
	m_h_Left[10] = -0.0000292139411613;
	m_h_Left[11] = -0.0000146474785652;
	m_h_Left[12] = 0.0000021514131276;
	m_h_Left[13] = 0.0000159797546075;
	m_h_Left[14] = 0.0000221549125854;
	m_h_Left[15] = 0.0000185557037184;
	m_h_Left[16] = 0.0000068430767897;
	m_h_Left[17] = -0.0000078581069829;
	m_h_Left[18] = -0.0000189057973330;
	m_h_Left[19] = -0.0000210210473597;
	m_h_Left[20] = -0.0000128407873490;
	m_h_Left[21] = 0.0000021356875095;
	m_h_Left[22] = 0.0000168605420185;
	m_h_Left[23] = 0.0000239175660681;
	m_h_Left[24] = 0.0000191849812836;
	m_h_Left[25] = 0.0000041909465835;
	m_h_Left[26] = -0.0000141687769428;
	m_h_Left[27] = -0.0000266804745479;
	m_h_Left[28] = -0.0000263464771706;
	m_h_Left[29] = -0.0000122119981825;
	m_h_Left[30] = 0.0000095742261692;
	m_h_Left[31] = 0.0000282986147795;
	m_h_Left[32] = 0.0000338566060236;
	m_h_Left[33] = 0.0000221132413571;
	m_h_Left[34] = -0.0000023518437047;
	m_h_Left[35] = -0.0000278249426628;
	m_h_Left[36] = -0.0000409598324040;
	m_h_Left[37] = -0.0000336766352120;
	m_h_Left[38] = -0.0000079384271885;
	m_h_Left[39] = 0.0000243380400207;
	m_h_Left[40] = 0.0000466642231913;
	m_h_Left[41] = 0.0000463036813017;
	m_h_Left[42] = 0.0000213879866351;
	m_h_Left[43] = -0.0000170829262061;
	m_h_Left[44] = -0.0000499074158142;
	m_h_Left[45] = -0.0000591524149058;
	m_h_Left[46] = -0.0000378185868612;
	m_h_Left[47] = 0.0000054448237279;
	m_h_Left[48] = 0.0000495497297379;
	m_h_Left[49] = 0.0000711093161954;
	m_h_Left[50] = 0.0000567109709664;
	m_h_Left[51] = 0.0000109462253022;
	m_h_Left[52] = -0.0000444776051154;
	m_h_Left[53] = -0.0000808542754385;
	m_h_Left[54] = -0.0000772124767536;
	m_h_Left[55] = -0.0000321749212162;
	m_h_Left[56] = 0.0000336578195856;
	m_h_Left[57] = 0.0000868840143085;
	m_h_Left[58] = 0.0000980972254183;
	m_h_Left[59] = 0.0000579376974201;
	m_h_Left[60] = -0.0000162688975252;
	m_h_Left[61] = -0.0000876213744050;
	m_h_Left[62] = -0.0001178134771180;
	m_h_Left[63] = -0.0000875171608641;
	m_h_Left[64] = -0.0000082237074821;
	m_h_Left[65] = 0.0000814914092189;
	m_h_Left[66] = 0.0001345096825389;
	m_h_Left[67] = 0.0001197328674607;
	m_h_Left[68] = 0.0000399582386308;
	m_h_Left[69] = -0.0000670430599712;
	m_h_Left[70] = -0.0001461128558731;
	m_h_Left[71] = -0.0001529414148536;
	m_h_Left[72] = -0.0000786032978795;
	m_h_Left[73] = 0.0000430524851254;
	m_h_Left[74] = 0.0001504003303126;
	m_h_Left[75] = 0.0001850285043474;
	m_h_Left[76] = 0.0001232606882695;
	m_h_Left[77] = -0.0000086719082901;
	m_h_Left[78] = -0.0001451331336284;
	m_h_Left[79] = -0.0002134631940862;
	m_h_Left[80] = -0.0001724150206428;
	m_h_Left[81] = -0.0000364416373486;
	m_h_Left[82] = 0.0001281958102481;
	m_h_Left[83] = 0.0002353741147090;
	m_h_Left[84] = 0.0002239009336336;
	m_h_Left[85] = 0.0000919931262615;
	m_h_Left[86] = -0.0000977704112302;
	m_h_Left[87] = -0.0002476811932866;
	m_h_Left[88] = -0.0002749281702563;
	m_h_Left[89] = -0.0001569569285493;
	m_h_Left[90] = 0.0000524965180375;
	m_h_Left[91] = 0.0002472368068993;
	m_h_Left[92] = 0.0003221260849386;
	m_h_Left[93] = 0.0002294945443282;
	m_h_Left[94] = 0.0000083545846792;
	m_h_Left[95] = -0.0002310073177796;
	m_h_Left[96] = -0.0003616396570578;
	m_h_Left[97] = -0.0003069138329010;
	m_h_Left[98] = -0.0000847206174512;
	m_h_Left[99] = 0.0001962697569979;
	m_h_Left[100] = 0.0003892597160302;
	m_h_Left[101] = 0.0003856586990878;
	m_h_Left[102] = 0.0001755936391419;
	m_h_Left[103] = -0.0001408394600730;
	m_h_Left[104] = -0.0004006113158539;
	m_h_Left[105] = -0.0004613618366420;
	m_h_Left[106] = -0.0002789076534100;
	m_h_Left[107] = 0.0000632931623841;
	m_h_Left[108] = 0.0003913739928976;
	m_h_Left[109] = 0.0005289468099363;
	m_h_Left[110] = 0.0003914636326954;
	m_h_Left[111] = 0.0000368107284885;
	m_h_Left[112] = -0.0003575389855541;
	m_h_Left[113] = -0.0005827903514728;
	m_h_Left[114] = -0.0005089115002193;
	m_h_Left[115] = -0.0001587389997439;
	m_h_Left[116] = 0.0002956803364214;
	m_h_Left[117] = 0.0006169307744130;
	m_h_Left[118] = 0.0006257825298235;
	m_h_Left[119] = 0.0003004157624673;
	m_h_Left[120] = -0.0002032436459558;
	m_h_Left[121] = -0.0006253321189433;
	m_h_Left[122] = -0.0007355912821367;
	m_h_Left[123] = -0.0004583036352415;
	m_h_Left[124] = 0.0000788305187598;
	m_h_Left[125] = 0.0006021905574016;
	m_h_Left[126] = 0.0008310037665069;
	m_h_Left[127] = 0.0006273413309827;
	m_h_Left[128] = 0.0000775332082412;
	m_h_Left[129] = -0.0005422755493782;
	m_h_Left[130] = -0.0009040769655257;
	m_h_Left[131] = -0.0008009540033527;
	m_h_Left[132] = -0.0002641671453603;
	m_h_Left[133] = 0.0004412865964696;
	m_h_Left[134] = 0.0009465577895753;
	m_h_Left[135] = 0.0009711356833577;
	m_h_Left[136] = 0.0004775526758749;
	m_h_Left[137] = -0.0002962144499179;
	m_h_Left[138] = -0.0009502378525212;
	m_h_Left[139] = -0.0011286106891930;
	m_h_Left[140] = -0.0007122103124857;
	m_h_Left[141] = 0.0001056927067111;
	m_h_Left[142] = 0.0009073557448573;
	m_h_Left[143] = 0.0012630778364837;
	m_h_Left[144] = 0.0009606519597583;
	m_h_Left[145] = 0.0001296803529840;
	m_h_Left[146] = -0.0008110329508781;
	m_h_Left[147] = -0.0013635336654261;
	m_h_Left[148] = -0.0012134213466197;
	m_h_Left[149] = -0.0004070691065863;
	m_h_Left[150] = 0.0006557236192748;
	m_h_Left[151] = 0.0014186699409038;
	m_h_Left[152] = 0.0014592278748751;
	m_h_Left[153] = 0.0007211973425001;
	m_h_Left[154] = -0.0004376557189971;
	m_h_Left[155] = -0.0014173235977069;
	m_h_Left[156] = -0.0016851695254445;
	m_h_Left[157] = -0.0010642196284607;
	m_h_Left[158] = 0.0001552503381390;
	m_h_Left[159] = 0.0013489727862179;
	m_h_Left[160] = 0.0018770446768031;
	m_h_Left[161] = 0.0014256818685681;
	m_h_Left[162] = 0.0001905011304189;
	m_h_Left[163] = -0.0012042585294694;
	m_h_Left[164] = -0.0020197455305606;
	m_h_Left[165] = -0.0017925744177774;
	m_h_Left[166] = -0.0005957146058790;
	m_h_Left[167] = 0.0009755205828696;
	m_h_Left[168] = 0.0020977298263460;
	m_h_Left[169] = 0.0021494934335351;
	m_h_Left[170] = 0.0010533761233091;
	m_h_Left[171] = -0.0006573111750185;
	m_h_Left[172] = -0.0020955423824489;
	m_h_Left[173] = -0.0024788931477815;
	m_h_Left[174] = -0.0015531908720732;
	m_h_Left[175] = 0.0002468732418492;
	m_h_Left[176] = 0.0019983709789813;
	m_h_Left[177] = 0.0027614291757345;
	m_h_Left[178] = 0.0020815343596041;
	m_h_Left[179] = 0.0002554488019086;
	m_h_Left[180] = -0.0017925972351804;
	m_h_Left[181] = -0.0029763551428914;
	m_h_Left[182] = -0.0026214842218906;
	m_h_Left[183] = -0.0008458612719551;
	m_h_Left[184] = 0.0014663392212242;
	m_h_Left[185] = 0.0031019740272313;
	m_h_Left[186] = 0.0031529506668448;
	m_h_Left[187] = 0.0015168727841228;
	m_h_Left[188] = -0.0010099314386025;
	m_h_Left[189] = -0.0031160889193416;
	m_h_Left[190] = -0.0036528608761728;
	m_h_Left[191] = -0.0022571331355721;
	m_h_Left[192] = 0.0004163304984104;
	m_h_Left[193] = 0.0029964295681566;
	m_h_Left[194] = 0.0040953969582915;
	m_h_Left[195] = 0.0030513887759298;
	m_h_Left[196] = 0.0003186264366377;
	m_h_Left[197] = -0.0027209594845772;
	m_h_Left[198] = -0.0044521889649332;
	m_h_Left[199] = -0.0038805003277957;
	m_h_Left[200] = -0.0011961093405262;
	m_h_Left[201] = 0.0022680191323161;
	m_h_Left[202] = 0.0046924264170229;
	m_h_Left[203] = 0.0047215311788023;
	m_h_Left[204] = 0.0022144161630422;
	m_h_Left[205] = -0.0016161559615284;
	m_h_Left[206] = -0.0047827241942286;
	m_h_Left[207] = -0.0055478112772107;
	m_h_Left[208] = -0.0033693523146212;
	m_h_Left[209] = 0.0007435118313879;
	m_h_Left[210] = 0.0046866005286574;
	m_h_Left[211] = 0.0063289487734437;
	m_h_Left[212] = 0.0046550347469747;
	m_h_Left[213] = 0.0003735880600289;
	m_h_Left[214] = -0.0043631680309772;
	m_h_Left[215] = -0.0070305466651917;
	m_h_Left[216] = -0.0060652270913124;
	m_h_Left[217] = -0.0017632801318541;
	m_h_Left[218] = 0.0037645003758371;
	m_h_Left[219] = 0.0076133953407407;
	m_h_Left[220] = 0.0075955823995173;
	m_h_Left[221] = 0.0034630466252565;
	m_h_Left[222] = -0.0028304771985859;
	m_h_Left[223] = -0.0080314734950662;
	m_h_Left[224] = -0.0092473234981298;
	m_h_Left[225] = -0.0055286134593189;
	m_h_Left[226] = 0.0014789294218645;
	m_h_Left[227] = 0.0082277162000537;
	m_h_Left[228] = 0.0110338740050793;
	m_h_Left[229] = 0.0080511523410678;
	m_h_Left[230] = 0.0004142033576500;
	m_h_Left[231] = -0.0081247519701719;
	m_h_Left[232] = -0.0129936235025525;
	m_h_Left[233] = -0.0111927380785346;
	m_h_Left[234] = -0.0030573662370443;
	m_h_Left[235] = 0.0076039703562856;
	m_h_Left[236] = 0.0152177270501852;
	m_h_Left[237] = 0.0152677698060870;
	m_h_Left[238] = 0.0068449787795544;
	m_h_Left[239] = -0.0064526787027717;
	m_h_Left[240] = -0.0179194156080484;
	m_h_Left[241] = -0.0209582298994064;
	m_h_Left[242] = -0.0126397209241986;
	m_h_Left[243] = 0.0042087305337191;
	m_h_Left[244] = 0.0216464996337891;
	m_h_Left[245] = 0.0300200320780277;
	m_h_Left[246] = 0.0227684658020735;
	m_h_Left[247] = 0.0004358185979072;
	m_h_Left[248] = -0.0281703099608421;
	m_h_Left[249] = -0.0485937669873238;
	m_h_Left[250] = -0.0463617891073227;
	m_h_Left[251] = -0.0134116141125560;
	m_h_Left[252] = 0.0474678650498390;
	m_h_Left[253] = 0.1222959980368614;
	m_h_Left[254] = 0.1901284903287888;
	m_h_Left[255] = 0.2303789854049683;
	m_h_Left[256] = 0.2303789854049683;
	m_h_Left[257] = 0.1901284903287888;
	m_h_Left[258] = 0.1222959980368614;
	m_h_Left[259] = 0.0474678650498390;
	m_h_Left[260] = -0.0134116141125560;
	m_h_Left[261] = -0.0463617891073227;
	m_h_Left[262] = -0.0485937669873238;
	m_h_Left[263] = -0.0281703099608421;
	m_h_Left[264] = 0.0004358185979072;
	m_h_Left[265] = 0.0227684658020735;
	m_h_Left[266] = 0.0300200320780277;
	m_h_Left[267] = 0.0216464996337891;
	m_h_Left[268] = 0.0042087305337191;
	m_h_Left[269] = -0.0126397209241986;
	m_h_Left[270] = -0.0209582298994064;
	m_h_Left[271] = -0.0179194156080484;
	m_h_Left[272] = -0.0064526787027717;
	m_h_Left[273] = 0.0068449787795544;
	m_h_Left[274] = 0.0152677698060870;
	m_h_Left[275] = 0.0152177270501852;
	m_h_Left[276] = 0.0076039703562856;
	m_h_Left[277] = -0.0030573662370443;
	m_h_Left[278] = -0.0111927380785346;
	m_h_Left[279] = -0.0129936235025525;
	m_h_Left[280] = -0.0081247519701719;
	m_h_Left[281] = 0.0004142033576500;
	m_h_Left[282] = 0.0080511523410678;
	m_h_Left[283] = 0.0110338740050793;
	m_h_Left[284] = 0.0082277162000537;
	m_h_Left[285] = 0.0014789294218645;
	m_h_Left[286] = -0.0055286134593189;
	m_h_Left[287] = -0.0092473234981298;
	m_h_Left[288] = -0.0080314734950662;
	m_h_Left[289] = -0.0028304771985859;
	m_h_Left[290] = 0.0034630466252565;
	m_h_Left[291] = 0.0075955823995173;
	m_h_Left[292] = 0.0076133953407407;
	m_h_Left[293] = 0.0037645003758371;
	m_h_Left[294] = -0.0017632801318541;
	m_h_Left[295] = -0.0060652270913124;
	m_h_Left[296] = -0.0070305466651917;
	m_h_Left[297] = -0.0043631680309772;
	m_h_Left[298] = 0.0003735880600289;
	m_h_Left[299] = 0.0046550347469747;
	m_h_Left[300] = 0.0063289487734437;
	m_h_Left[301] = 0.0046866005286574;
	m_h_Left[302] = 0.0007435118313879;
	m_h_Left[303] = -0.0033693523146212;
	m_h_Left[304] = -0.0055478112772107;
	m_h_Left[305] = -0.0047827241942286;
	m_h_Left[306] = -0.0016161559615284;
	m_h_Left[307] = 0.0022144161630422;
	m_h_Left[308] = 0.0047215311788023;
	m_h_Left[309] = 0.0046924264170229;
	m_h_Left[310] = 0.0022680191323161;
	m_h_Left[311] = -0.0011961093405262;
	m_h_Left[312] = -0.0038805003277957;
	m_h_Left[313] = -0.0044521889649332;
	m_h_Left[314] = -0.0027209594845772;
	m_h_Left[315] = 0.0003186264366377;
	m_h_Left[316] = 0.0030513887759298;
	m_h_Left[317] = 0.0040953969582915;
	m_h_Left[318] = 0.0029964295681566;
	m_h_Left[319] = 0.0004163304984104;
	m_h_Left[320] = -0.0022571331355721;
	m_h_Left[321] = -0.0036528608761728;
	m_h_Left[322] = -0.0031160889193416;
	m_h_Left[323] = -0.0010099314386025;
	m_h_Left[324] = 0.0015168727841228;
	m_h_Left[325] = 0.0031529506668448;
	m_h_Left[326] = 0.0031019740272313;
	m_h_Left[327] = 0.0014663392212242;
	m_h_Left[328] = -0.0008458612719551;
	m_h_Left[329] = -0.0026214842218906;
	m_h_Left[330] = -0.0029763551428914;
	m_h_Left[331] = -0.0017925972351804;
	m_h_Left[332] = 0.0002554488019086;
	m_h_Left[333] = 0.0020815343596041;
	m_h_Left[334] = 0.0027614291757345;
	m_h_Left[335] = 0.0019983709789813;
	m_h_Left[336] = 0.0002468732418492;
	m_h_Left[337] = -0.0015531908720732;
	m_h_Left[338] = -0.0024788931477815;
	m_h_Left[339] = -0.0020955423824489;
	m_h_Left[340] = -0.0006573111750185;
	m_h_Left[341] = 0.0010533761233091;
	m_h_Left[342] = 0.0021494934335351;
	m_h_Left[343] = 0.0020977298263460;
	m_h_Left[344] = 0.0009755205828696;
	m_h_Left[345] = -0.0005957146058790;
	m_h_Left[346] = -0.0017925744177774;
	m_h_Left[347] = -0.0020197455305606;
	m_h_Left[348] = -0.0012042585294694;
	m_h_Left[349] = 0.0001905011304189;
	m_h_Left[350] = 0.0014256818685681;
	m_h_Left[351] = 0.0018770446768031;
	m_h_Left[352] = 0.0013489727862179;
	m_h_Left[353] = 0.0001552503381390;
	m_h_Left[354] = -0.0010642196284607;
	m_h_Left[355] = -0.0016851695254445;
	m_h_Left[356] = -0.0014173235977069;
	m_h_Left[357] = -0.0004376557189971;
	m_h_Left[358] = 0.0007211973425001;
	m_h_Left[359] = 0.0014592278748751;
	m_h_Left[360] = 0.0014186699409038;
	m_h_Left[361] = 0.0006557236192748;
	m_h_Left[362] = -0.0004070691065863;
	m_h_Left[363] = -0.0012134213466197;
	m_h_Left[364] = -0.0013635336654261;
	m_h_Left[365] = -0.0008110329508781;
	m_h_Left[366] = 0.0001296803529840;
	m_h_Left[367] = 0.0009606519597583;
	m_h_Left[368] = 0.0012630778364837;
	m_h_Left[369] = 0.0009073557448573;
	m_h_Left[370] = 0.0001056927067111;
	m_h_Left[371] = -0.0007122103124857;
	m_h_Left[372] = -0.0011286106891930;
	m_h_Left[373] = -0.0009502378525212;
	m_h_Left[374] = -0.0002962144499179;
	m_h_Left[375] = 0.0004775526758749;
	m_h_Left[376] = 0.0009711356833577;
	m_h_Left[377] = 0.0009465577895753;
	m_h_Left[378] = 0.0004412865964696;
	m_h_Left[379] = -0.0002641671453603;
	m_h_Left[380] = -0.0008009540033527;
	m_h_Left[381] = -0.0009040769655257;
	m_h_Left[382] = -0.0005422755493782;
	m_h_Left[383] = 0.0000775332082412;
	m_h_Left[384] = 0.0006273413309827;
	m_h_Left[385] = 0.0008310037665069;
	m_h_Left[386] = 0.0006021905574016;
	m_h_Left[387] = 0.0000788305187598;
	m_h_Left[388] = -0.0004583036352415;
	m_h_Left[389] = -0.0007355912821367;
	m_h_Left[390] = -0.0006253321189433;
	m_h_Left[391] = -0.0002032436459558;
	m_h_Left[392] = 0.0003004157624673;
	m_h_Left[393] = 0.0006257825298235;
	m_h_Left[394] = 0.0006169307744130;
	m_h_Left[395] = 0.0002956803364214;
	m_h_Left[396] = -0.0001587389997439;
	m_h_Left[397] = -0.0005089115002193;
	m_h_Left[398] = -0.0005827903514728;
	m_h_Left[399] = -0.0003575389855541;
	m_h_Left[400] = 0.0000368107284885;
	m_h_Left[401] = 0.0003914636326954;
	m_h_Left[402] = 0.0005289468099363;
	m_h_Left[403] = 0.0003913739928976;
	m_h_Left[404] = 0.0000632931623841;
	m_h_Left[405] = -0.0002789076534100;
	m_h_Left[406] = -0.0004613618366420;
	m_h_Left[407] = -0.0004006113158539;
	m_h_Left[408] = -0.0001408394600730;
	m_h_Left[409] = 0.0001755936391419;
	m_h_Left[410] = 0.0003856586990878;
	m_h_Left[411] = 0.0003892597160302;
	m_h_Left[412] = 0.0001962697569979;
	m_h_Left[413] = -0.0000847206174512;
	m_h_Left[414] = -0.0003069138329010;
	m_h_Left[415] = -0.0003616396570578;
	m_h_Left[416] = -0.0002310073177796;
	m_h_Left[417] = 0.0000083545846792;
	m_h_Left[418] = 0.0002294945443282;
	m_h_Left[419] = 0.0003221260849386;
	m_h_Left[420] = 0.0002472368068993;
	m_h_Left[421] = 0.0000524965180375;
	m_h_Left[422] = -0.0001569569285493;
	m_h_Left[423] = -0.0002749281702563;
	m_h_Left[424] = -0.0002476811932866;
	m_h_Left[425] = -0.0000977704112302;
	m_h_Left[426] = 0.0000919931262615;
	m_h_Left[427] = 0.0002239009336336;
	m_h_Left[428] = 0.0002353741147090;
	m_h_Left[429] = 0.0001281958102481;
	m_h_Left[430] = -0.0000364416373486;
	m_h_Left[431] = -0.0001724150206428;
	m_h_Left[432] = -0.0002134631940862;
	m_h_Left[433] = -0.0001451331336284;
	m_h_Left[434] = -0.0000086719082901;
	m_h_Left[435] = 0.0001232606882695;
	m_h_Left[436] = 0.0001850285043474;
	m_h_Left[437] = 0.0001504003303126;
	m_h_Left[438] = 0.0000430524851254;
	m_h_Left[439] = -0.0000786032978795;
	m_h_Left[440] = -0.0001529414148536;
	m_h_Left[441] = -0.0001461128558731;
	m_h_Left[442] = -0.0000670430599712;
	m_h_Left[443] = 0.0000399582386308;
	m_h_Left[444] = 0.0001197328674607;
	m_h_Left[445] = 0.0001345096825389;
	m_h_Left[446] = 0.0000814914092189;
	m_h_Left[447] = -0.0000082237074821;
	m_h_Left[448] = -0.0000875171608641;
	m_h_Left[449] = -0.0001178134771180;
	m_h_Left[450] = -0.0000876213744050;
	m_h_Left[451] = -0.0000162688975252;
	m_h_Left[452] = 0.0000579376974201;
	m_h_Left[453] = 0.0000980972254183;
	m_h_Left[454] = 0.0000868840143085;
	m_h_Left[455] = 0.0000336578195856;
	m_h_Left[456] = -0.0000321749212162;
	m_h_Left[457] = -0.0000772124767536;
	m_h_Left[458] = -0.0000808542754385;
	m_h_Left[459] = -0.0000444776051154;
	m_h_Left[460] = 0.0000109462253022;
	m_h_Left[461] = 0.0000567109709664;
	m_h_Left[462] = 0.0000711093161954;
	m_h_Left[463] = 0.0000495497297379;
	m_h_Left[464] = 0.0000054448237279;
	m_h_Left[465] = -0.0000378185868612;
	m_h_Left[466] = -0.0000591524149058;
	m_h_Left[467] = -0.0000499074158142;
	m_h_Left[468] = -0.0000170829262061;
	m_h_Left[469] = 0.0000213879866351;
	m_h_Left[470] = 0.0000463036813017;
	m_h_Left[471] = 0.0000466642231913;
	m_h_Left[472] = 0.0000243380400207;
	m_h_Left[473] = -0.0000079384271885;
	m_h_Left[474] = -0.0000336766352120;
	m_h_Left[475] = -0.0000409598324040;
	m_h_Left[476] = -0.0000278249426628;
	m_h_Left[477] = -0.0000023518437047;
	m_h_Left[478] = 0.0000221132413571;
	m_h_Left[479] = 0.0000338566060236;
	m_h_Left[480] = 0.0000282986147795;
	m_h_Left[481] = 0.0000095742261692;
	m_h_Left[482] = -0.0000122119981825;
	m_h_Left[483] = -0.0000263464771706;
	m_h_Left[484] = -0.0000266804745479;
	m_h_Left[485] = -0.0000141687769428;
	m_h_Left[486] = 0.0000041909465835;
	m_h_Left[487] = 0.0000191849812836;
	m_h_Left[488] = 0.0000239175660681;
	m_h_Left[489] = 0.0000168605420185;
	m_h_Left[490] = 0.0000021356875095;
	m_h_Left[491] = -0.0000128407873490;
	m_h_Left[492] = -0.0000210210473597;
	m_h_Left[493] = -0.0000189057973330;
	m_h_Left[494] = -0.0000078581069829;
	m_h_Left[495] = 0.0000068430767897;
	m_h_Left[496] = 0.0000185557037184;
	m_h_Left[497] = 0.0000221549125854;
	m_h_Left[498] = 0.0000159797546075;
	m_h_Left[499] = 0.0000021514131276;
	m_h_Left[500] = -0.0000146474785652;
	m_h_Left[501] = -0.0000292139411613;
	m_h_Left[502] = -0.0000377439646400;
	m_h_Left[503] = -0.0000389039269066;
	m_h_Left[504] = -0.0000337998826581;
	m_h_Left[505] = -0.0000250798548223;
	m_h_Left[506] = -0.0000157045305968;
	m_h_Left[507] = -0.0000079230067058;
	m_h_Left[508] = -0.0000027811349810;
	m_h_Left[509] = -0.0000002019931742;
	m_h_Left[510] = 0.0000005707121318;
	m_h_Left[511] = 0.0000005445158422;
}

CrashSyncAudioProcessor::~CrashSyncAudioProcessor()
{
	if(m_pLeftInterpBuffer) delete[] m_pLeftInterpBuffer;
	if(m_pRightInterpBuffer) delete[] m_pRightInterpBuffer;
	if(m_pLeftDecipBuffer) delete[] m_pLeftDecipBuffer;
	if(m_pRightDeciBuffer) delete[] m_pRightDeciBuffer;
}

//==============================================================================
const juce::String CrashSyncAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CrashSyncAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CrashSyncAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CrashSyncAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CrashSyncAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CrashSyncAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CrashSyncAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CrashSyncAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CrashSyncAudioProcessor::getProgramName (int index)
{
    return {};
}

void CrashSyncAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CrashSyncAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_EnvelopeFollowerL.setSamplerate(sampleRate);
	m_EnvelopeFollowerR.setSamplerate(sampleRate);
    m_OscillatorL.setSamplerate(sampleRate);
    m_OscillatorL.reset(false);
	m_OscillatorR.setSamplerate(sampleRate);
	m_OscillatorR.reset(false);
	m_FilterL.setSamplerate(sampleRate);
	m_FilterL.reset();
	m_FilterR.setSamplerate(sampleRate);
	m_FilterR.reset();
	m_Interpolator.reset();
	m_Decimator.reset();
}

void CrashSyncAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CrashSyncAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CrashSyncAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // subrate
    m_OscillatorL.setFrequency((m_pFrequency->get() * m_pFrequency->get()) * 19000 + 1000);
    m_OscillatorL.setWaveform(static_cast<int>(m_pWaveform->get() * (SCOscillator::numWaveforms - 1) + 0.5f));
    m_OscillatorL.setApplyPolyBlep(m_pPolyBlep->get());
    m_OscillatorL.setPulseWidth(m_pPulseWidth->get());
	m_OscillatorR.setFrequency((m_pFrequency->get() * m_pFrequency->get()) * 19000 + 1000);
	m_OscillatorR.setWaveform(static_cast<int>(m_pWaveform->get() * (SCOscillator::numWaveforms - 1) + 0.5f));
	m_OscillatorR.setApplyPolyBlep(m_pPolyBlep->get());
	m_OscillatorR.setPulseWidth(m_pPulseWidth->get());
    m_EnvelopeFollowerL.setAttackTimeMs(m_pEnvAttack->get());
    m_EnvelopeFollowerL.setReleaseTimeMs(m_pEnvRelease->get());
	m_EnvelopeFollowerR.setAttackTimeMs(m_pEnvAttack->get());
	m_EnvelopeFollowerR.setReleaseTimeMs(m_pEnvRelease->get());

    float cutoff = m_pTone->get() * m_pTone->get();
    cutoff = (20.f + (19980.f * cutoff)) / getSampleRate();

    const float* inputL = buffer.getReadPointer(0);
    const float* inputR = buffer.getReadPointer(1);
    float* outputL = buffer.getWritePointer(0);
    float* outputR = buffer.getWritePointer(1);

    for(int i = 0; i < buffer.getNumSamples(); i++)
    {
		m_Interpolator.interpolateSamples(*(inputL + i), *(inputR + i), m_pLeftInterpBuffer, m_pRightInterpBuffer);
		for(int j = 0; j < m_nOversamplingRatio; j++)
		{
			float sigL = m_pLeftInterpBuffer[j];
			float sigR = m_pRightInterpBuffer[j];

			// fuzz section
			float gain = 0.5f + 39.5f * m_pGain->get();
			sigL *= gain;
			sigL = (sigL > 1.f) ? 1.f : sigL;
			sigL = (sigL < 0) ? -1.f : sigL;

			sigR *= gain;
			sigR = (sigR > 1.f) ? 1.f : sigR;
			sigR = (sigR < 0) ? -1.f : sigR;

			if(m_pInputMode->get() == kInputModeEnvelope)
			{
				sigL = m_EnvelopeFollowerL.process(sigL);
				sigR = m_EnvelopeFollowerR.process(sigR);
			}

			// check for reset
			if(sigL <= m_pThreshold->get())
			{
				m_OscillatorL.reset(true);
			}
			else
			{
				m_OscillatorL.setResetState(false);
			}

			if(sigL <= m_pThreshold->get())
			{
				m_OscillatorR.reset(true);
			}
			else
			{
				m_OscillatorR.setResetState(false);
			}

			m_pLeftDecipBuffer[i] = sigL;
			m_pRightDeciBuffer[i] = sigR;
		}
		
		float outL, outR;
		// decimate them
		m_Decimator.decimateSamples(m_pLeftDecipBuffer, m_pRightDeciBuffer, outL, outR);

        *outputL++ = m_FilterL.process(outL) * m_pOutputVolume->get();
        *outputR++ = m_FilterL.process(outR) * m_pOutputVolume->get();
    }
}

//==============================================================================
bool CrashSyncAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CrashSyncAudioProcessor::createEditor()
{
    return new CrashSyncAudioProcessorEditor (*this);
}

//==============================================================================
void CrashSyncAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CrashSyncAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrashSyncAudioProcessor();
}
