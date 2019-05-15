/**************************************************************************
  syntbl.cpp - Tables for synthesis filterbank.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*
   Purpose:     Offset table describing the locations of the DCT samples to
                be windowed for each subwindow.
   Explanation: - */
static int16 buf_idx_offset[16][16] = {
    { 0, 96, 128, 224, 256, 352, 384, 480, 512, 608, 640, 736, 768, 864, 896, 992 },
    { 960, 32, 64, 160, 192, 288, 320, 416, 448, 544, 576, 672, 704, 800, 832, 928 },
    { 896, 992, 0, 96, 128, 224, 256, 352, 384, 480, 512, 608, 640, 736, 768, 864 },
    { 832, 928, 960, 32, 64, 160, 192, 288, 320, 416, 448, 544, 576, 672, 704, 800 },
    { 768, 864, 896, 992, 0, 96, 128, 224, 256, 352, 384, 480, 512, 608, 640, 736 },
    { 704, 800, 832, 928, 960, 32, 64, 160, 192, 288, 320, 416, 448, 544, 576, 672 },
    { 640, 736, 768, 864, 896, 992, 0, 96, 128, 224, 256, 352, 384, 480, 512, 608 },
    { 576, 672, 704, 800, 832, 928, 960, 32, 64, 160, 192, 288, 320, 416, 448, 544 },
    { 512, 608, 640, 736, 768, 864, 896, 992, 0, 96, 128, 224, 256, 352, 384, 480 },
    { 448, 544, 576, 672, 704, 800, 832, 928, 960, 32, 64, 160, 192, 288, 320, 416 },
    { 384, 480, 512, 608, 640, 736, 768, 864, 896, 992, 0, 96, 128, 224, 256, 352 },
    { 320, 416, 448, 544, 576, 672, 704, 800, 832, 928, 960, 32, 64, 160, 192, 288 },
    { 256, 352, 384, 480, 512, 608, 640, 736, 768, 864, 896, 992, 0, 96, 128, 224 },
    { 192, 288, 320, 416, 448, 544, 576, 672, 704, 800, 832, 928, 960, 32, 64, 160 },
    { 128, 224, 256, 352, 384, 480, 512, 608, 640, 736, 768, 864, 896, 992, 0, 96 },
    { 64, 160, 192, 288, 320, 416, 448, 544, 576, 672, 704, 800, 832, 928, 960, 32 }
};

#ifdef SCALED_DCT
/*
   Purpose:     Synthesis window for scaled DCT.
   Explanation: These are multiplied by the overall scale factor. Also some
                of the sign bits are flipped which simplifies the re-ordering
                of the DCT samples. */
static FLOAT dewindow[HAN_SIZE] = {
    +0.00000000000000000,     -0.37048068346560115,     -0.38651059086424766,
    -0.40160934162609863,     -0.41574057641506101,     -0.42887023070559849,
    -0.44096674340187292,     -0.90400181564814375,     -0.92389233947150728,
    -0.94155707587128745,     -0.95695358575355272,     -1.45503521285564830,
    -1.47116615661187922,     -1.97834795301079680,     -1.99036433780873523,
    -2.49699052938547084,     -0.00000000000000000,     +2.99639516006270412,
    +3.48316194105309762,     +3.46213312064963930,     +3.92313118606368505,
    +4.36513743866976256,     +4.78470541502133528,     +5.17850218485318692,
    +6.00520923503296444,     +6.32792391206362481,     +7.05538126466444737,
    +7.29068165235480592,     +7.89896172285079956,     +8.43369104086269772,
    +9.27612738229845490,     +9.63237658373742534,     +10.25305084912974962,
    +11.48473106721121440,    +13.52769289340596970,    +15.26094281673431396,
    +17.04511719094739419,    +19.29890659919510654,    +21.60708027058103653,
    +23.95572202104176540,    +26.79251267055133212,    +29.65864674133354128,
    +32.53598295048163891,    +35.40615214383615239,    +38.74100617702197269,
    +42.03999797658343596,    +45.28091233275199556,    +48.44159455249518942,
    +51.99999618530273438,    +55.43316270063132834,    +58.21829470542274976,
    +61.82354330550970190,    +64.73181906101035566,    +67.41718007111558108,
    +70.33511593937873840,    +72.49890667410363676,    +74.37230008840560913,
    +76.38708332650276134,    +77.60907512223229787,    +78.48215385803450772,
    +78.98961156606674194,    +78.71434276166928612,    +78.07404190953548096,
    +77.05891091574767416,    +75.30687090754508972,    +80.76367217302322388,
    +85.80415875045400753,    +90.36086458689806022,    +94.37179950308109255,
    +97.78106045722961426,    +100.53902721405029297,   +102.60278035934970831,
    +103.47451819518710181,   +104.04060381440649508,   +102.87110254029812495,
    +100.88324378943434567,   +98.07853850963829245,    +93.47717281492987240,
    +88.07385527394080782,    +81.40184439370841574,    +0.00000000000000000,
    -63.42350343788757528,    -52.74480152579940296,    -41.05081014459210564,
    -27.95238134264945984,    -14.06545699131493166,    +0.95695358575355272,
    +16.94778043592759786,    +33.25967134574557349,    +50.17142077811149648,
    +67.46698551690133172,    +84.48625406154860684,    +101.43931060172508296,
    +118.07151371240615840,   +134.11732041835784910,   +148.56068842509466780,
    +162.28100350499153140,   +192.27683847349726420,   +224.55953070521354680,
    +259.03442412614822390,   +295.58744117617607120,   +334.08528989553451540,
    +373.93460226058959960,   +415.38309025764465330,   +457.78229433298110960,
    +500.90143775939941400,   +544.02059063315391540,   +586.86891317367553710,
    +629.17373830080032350,   +670.66167497634887700,   +710.56188976764678960,
    +748.09779185056686400,   +783.50000000000000000,   +816.01588243246078490,
    +844.91182690858840940,   +869.98074209690094000,   +891.04340022802352900,
    +906.97922945022583010,   +918.18426862359046940,   +923.65471887588500980,
    +924.34144395589828490,   +918.45314407348632810,   +907.05598685145378120,
    +889.89342302083969120,   +866.80705323815345760,   +838.14704364538192750,
    +803.93085002899169920,   +764.29108116030693060,   +720.18824431300163270,
    +740.95112085342407220,   +754.45824050903320310,   +760.23597094416618340,
    +757.46885573863983160,   +745.79501813650131220,   +724.93930149078369140,
    +693.81179928779602050,   +653.18285161256790160,   +602.58819580078125000,
    +541.14976957440376280,   +470.46516180038452150,   +389.37179810774796350,
    +299.22589480876922610,   +200.03212863206863400,   +92.38857910037040710,
    -0.00000000000000000,     +143.82652952124863080,   +271.18783608078956600,
    +402.59484028816223140,   +536.97995737195014960,   +673.20169544219970700,
    +809.57153856754302980,   +944.36868810653686520,   +1076.31969183683395400,
    +1203.66177296638488800,  +1324.64577960968017600,  +1437.12427014112472500,
    +1540.29753595590591400,  +1631.71619120240211500,  +1710.28571426868438700,
    +1773.83698332309722900,  +1821.86059054732322700,  +2043.91366687417030300,
    +2272.26416698098182700,  +2504.80264073610305800,  +2739.27658215165138200,
    +2974.17392224073410000,  +3206.22463801503181400,  +3433.80341315269470200,
    +3653.94343912601471000,  +3864.56757760047912600,  +4062.69026830792427100,
    +4246.31184697151184100,  +4412.55284273624420200,  +4559.60913097858429000,
    +4685.32965970039367700,  +4786.72719284892082200,  +4863.50000000000000000,
    +4913.07481724023819000,  +4934.12583959102630600,  +4925.60444033145904600,
    +4887.25290405750274600,  +4818.63029122352600100,  +4719.15135040879249600,
    +4590.02727270126342800,  +4430.92610216140747100,  +4243.77783942222595200,
    +4029.93906846642494200,  +3791.16041421890258800,  +3530.41990220546722400,
    +3249.77761244773864800,  +2952.89985299110412600,  +2642.97264808416366600,
    +2324.25994998216629000,  +2207.66386458277702300,  +2043.83974170684814400,
    +1831.71487769484520000,  +1569.81470870971679700,  +1258.71672123670578000,
    +898.23683524131774900,   +489.05821943283081060,   +32.33577768465374902,
    -469.83045517977734560,   -1015.31371444463729900,  -1600.55158138275146500,
    -2222.94990572333335900,  -2877.51447272300720200,  -3559.77574187517166200,
    -4264.85657036304473900,  -0.00000000000000000,     +5720.60093834996223400,
    +6458.74882996082305900,  +7195.26994657516479500,  +7922.29334369301796000,
    +8632.79321122169494600,  +9319.64209705591201800,  +9975.18850064277648900,
    +10591.81725928187370000, +11161.55607271194458000, +11677.51981878280640000,
    +12132.14218980073929000, +12518.60712623596191000, +12830.03621962666512000,
    +13060.39877307415008000, +13203.74897360801697000, +13254.36283311247826000,
    +14573.02664494514465000, +15914.73878884315491000, +17271.37123095989227000,
    +18633.64936843514442000, +19992.36708480119705000, +21338.08420211076736000,
    +22661.65614390373230000, +23952.96196573972702000, +25202.30974531173706000,
    +26401.02739661931992000, +27538.21748113632202000, +28606.07301348447800000,
    +29595.17205548286438000, +30496.93813863396644000, +31304.24698555469513000,
    +32009.50000000000000000, +32605.67745655775070000, +33086.90637129545212000,
    +33448.50915706157684000, +33686.05021929740906000, +33795.40413308143616000,
    +33775.20975619554520000, +33622.53823757171631000, +33337.72996944189072000,
    +32921.03088021278381000, +32373.12361434102058000, +31696.50270295143128000,
    +30894.50002267956734000, +29971.68848454952240000, +28932.23447585105896000,
    +27782.70322751998901000, +26529.93886929750442000, +27782.70322751998901000,
    +28932.23670673370361000, +29971.69070869684219000, +30894.50223737955094000,
    +31696.50270295143128000, +32373.12580227851868000, +32921.03088021278381000,
    +33337.73212024569512000, +33622.53823757171631000, +33775.20975619554520000,
    +33795.40413308143616000, +33686.05226647853851000, +33448.50915706157684000,
    +33086.90637129545212000, +32605.67745655775070000, +0.00000000000000000,
    -31304.24698555469513000, -30496.93813863396644000, -29595.17205548286438000,
    -28606.07475194334984000, -27538.21748113632202000, -26401.02739661931992000,
    -25202.30974531173706000, -23952.96351107954979000, -22661.65614390373230000,
    -21338.08564424514770000, -19992.36708480119705000, -18633.65070420503616000,
    -17271.37251263856888000, -15914.74001598358154000, -14573.02664494514465000,
    -13254.36283311247826000, -13203.74897360801697000, -13060.39776602387428000,
    -12830.03526753187180000, -12518.60622882843018000, -12132.14218980073929000,
    -11677.51902955770492000, -11161.55607271194458000, -10591.81657594442368000,
    -9975.18850064277648900,  -9319.64209705591201800,  -8632.79321122169494600,
    -7922.29286223649978600,  -7195.26994657516479500,  -6458.74882996082305900,
    -5720.60093834996223400,  -4987.50000000000000000,  -4264.85657036304473900,
    -3559.77574187517166200,  -2877.51447272300720200,  -2222.94977062940597500,
    -1600.55158138275146500,  -1015.31371444463729900,  -469.83045517977734560,
    +32.33577559849163663,    +489.05821943283081060,   +898.23677453398704530,
    +1258.71672123670578000,  +1569.81459617614746100,  +1831.71474176645278900,
    +2043.83958411216735800,  +2207.66386458277702300,  +2324.25994998216629000,
    +2642.97264808416366600,  +2952.90008068084716800,  +3249.77785360813140900,
    +3530.42015528678894000,  +3791.16041421890258800,  +4029.93934082984924300,
    +4243.77783942222595200,  +4430.92638802528381400,  +4590.02727270126342800,
    +4719.15135040879249600,  +4818.63029122352600100,  +4887.25320106744766200,
    +4925.60444033145904600,  +4934.12583959102630600,  +4913.07481724023819000,
    +0.00000000000000000,     -4786.72719284892082200,  -4685.32965970039367700,
    -4559.60913097858429000,  -4412.55311089754104600,  -4246.31184697151184100,
    -4062.69026830792427100,  -3864.56757760047912600,  -3653.94367486238479600,
    -3433.80341315269470200,  -3206.22485470771789600,  -2974.17392224073410000,
    -2739.27677851915359500,  -2504.80282661318779000,  -2272.26434218883514400,
    -2043.91366687417030300,  -1821.86059054732322700,  -1773.83698332309722900,
    -1710.28558239340782200,  -1631.71607011556625400,  -1540.29742553830146800,
    -1437.12427014112472500,  -1324.64569008350372300,  -1203.66177296638488800,
    -1076.31962239742279000,  -944.36868810653686520,   -809.57153856754302980,
    -673.20169544219970700,   -536.97992473840713500,   -402.59484028816223140,
    -271.18783608078956600,   -143.82652952124863080,   -22.50001525878906250,
    +92.38857910037040710,    +200.03212863206863400,   +299.22589480876922610,
    +389.37177444470216870,   +470.46516180038452150,   +541.14976957440376280,
    +602.58819580078125000,   +653.18280947208404540,   +693.81179928779602050,
    +724.93925249576568600,   +745.79501813650131220,   +757.46880143880844120,
    +760.23591452836990360,   +754.45818233489990240,   +740.95112085342407220,
    +720.18824431300163270,   +764.29108116030693060,   +803.93091201782226560,
    +838.14710584282875060,   +866.80711537599563600,   +889.89342302083969120,
    +907.05604815483093260,   +918.45314407348632810,   +924.34150359034538270,
    +923.65471887588500980,   +918.18426862359046940,   +906.97922945022583010,
    +891.04345437884330750,   +869.98074209690094000,   +844.91182690858840940,
    +816.01588243246078490,   +0.00000000000000000,     -748.09779185056686400,
    -710.56188976764678960,   -670.66167497634887700,   -629.17377653717994690,
    -586.86891317367553710,   -544.02059063315391540,   -500.90143775939941400,
    -457.78232386708259580,   -415.38309025764465330,   -373.93462753295898440,
    -334.08528989553451540,   -295.58746236562728880,   -259.03444334864616400,
    -224.55954802036285400,   -192.27683847349726420,   -162.28100350499153140,
    -148.56068842509466780,   -134.11731007695198060,   -118.07150495052337650,
    -101.43930332995751087,   -84.48625406154860684,    -67.46698095714555166,
    -50.17142077811149648,    -33.25966919997790683,    -16.94778043592759786,
    -0.95695358575355272,     +14.06545699131493166,    +27.95237964391708374,
    +41.05081014459210564,    +52.74480152579940296,    +63.42350343788757528,
    +72.99999237060546875,    +81.40184439370841574,    +88.07385527394080782,
    +93.47717281492987240,    +98.07853254917336017,    +100.88324378943434567,
    +102.87110254029812495,   +104.04060381440649508,   +103.47451151946643222,
    +102.60278035934970831,   +100.53902041912078857,   +97.78106045722961426,
    +94.37179273795436529,    +90.36085788137461350,    +85.80415213433889221,
    +80.76367217302322388,    +75.30687090754508972,    +77.05891091574767416,
    +78.07404792960369378,    +78.71434860292492886,    +78.98961722850799560,
    +78.48215385803450772,    +77.60908036744149286,    +76.38708332650276134,
    +74.37230488657951355,    +72.49890667410363676,    +70.33511593937873840,
    +67.41718007111558108,    +64.73182299491645608,    +61.82354330550970190,
    +58.21829470542274976,    +55.43316270063132834,    +0.00000000000000000,
    -48.44159455249518942,    -45.28091233275199556,    -42.03999797658343596,
    -38.74100853140475920,    -35.40615214383615239,    -32.53598295048163891,
    -29.65864674133354128,    -26.79251439908648536,    -23.95572202104176540,
    -21.60708173089551565,    -19.29890659919510654,    -17.04511841284192997,
    -15.26094394922256470,    -13.52769393648804908,    -11.48473106721121440,
    -10.25305084912974962,    -9.63237658373742534,     -9.27612666704260391,
    -8.43369041501307493,     -7.89896115660667419,     -7.29068165235480592,
    -7.05538078782655020,     -6.32792391206362481,     -6.00520884760328499,
    -5.17850218485318692,     -4.78470541502133528,     -4.36513743866976256,
    -3.92313094764571702,     -3.46213312064963930,     -3.48316194105309762,
    -2.99639516006270412,     -2.50000190734863281,     -2.49699052938547084,
    -1.99036433780873523,     -1.97834795301079680,     -1.47116606720562970,
    -1.45503521285564830,     -0.95695358575355272,     -0.94155707587128745,
    -0.92389227986603828,     -0.90400181564814375,     -0.44096671359913842,
    -0.42887023070559849,     -0.41574054661232651,     -0.40160931182336412,
    -0.38651056106151316,     -0.37048068346560115
};
#else
/*
   Purpose:     Synthesis window as specified by the standard.
   Explanation: These are multiplied by the overall scale factor. */
static FLOAT dewindow[HAN_SIZE] = {
    +0.000000,     -0.500007,     -0.500007,     -0.500007,     -0.500007,
    -0.500007,     -0.500007,     -1.000014,     -1.000014,     -1.000014,
    -1.000014,     -1.499988,     -1.499988,     -1.999995,     -1.999995,
    -2.500002,     -2.500002,     -3.000009,     -3.500015,     -3.500015,
    -3.999990,     -4.499997,     -5.000004,     -5.500010,     -6.499991,
    -6.999999,     -8.000012,     -8.499987,     -9.500000,     -10.500014,
    -12.000002,    -13.000016,    -14.500004,    -15.499985,    -17.500013,
    -19.000000,    -20.499989,    -22.500015,    -24.500011,    -26.500006,
    -29.000008,    -31.500010,    -34.000011,    -36.500011,    -39.499989,
    -42.499996,    -45.500008,    -48.500015,    -51.999996,    -55.500015,
    -58.499989,    -62.500011,    -65.999992,    -69.500008,    -73.500000,
    -77.000015,    -80.500000,    -84.499985,    -88.000008,    -91.499985,
    -95.000000,    -98.000008,    -100.999985,   -103.999992,   +106.500000,
    +109.000000,   +110.999992,   +112.500015,   +113.499992,   +114.000000,
    +114.000000,   +113.499992,   +112.000008,   +110.499985,   +107.500015,
    +103.999992,   +100.000008,   +94.499992,    +88.500008,    +81.500015,
    +72.999992,    +63.499992,    +53.000011,    +41.499985,    +28.500000,
    +14.500004,    -1.000014,     -17.999987,    -36.000008,    -55.500015,
    -76.500008,    -98.499985,    -122.000015,   -147.000000,   -173.500000,
    -200.499985,   -229.500000,   -259.500031,   -290.500000,   -322.500000,
    -355.500000,   -389.500000,   -424.000000,   -459.500000,   -495.500000,
    -532.000000,   -568.500000,   -605.000000,   -641.500000,   -678.000000,
    -714.000000,   -749.000000,   -783.500000,   -817.000000,   -849.000000,
    -879.500000,   -908.500000,   -935.000000,   -959.500000,   -981.000000,
    -1000.500000,  -1016.000000,  -1028.500000,  -1037.500000,  -1042.500000,
    -1043.500000,  -1040.000000,  -1031.500000,  +1018.500000,  +1000.000000,
    +976.000000,   +946.500000,   +911.000000,   +869.500000,   +822.000000,
    +767.500000,   +707.000000,   +640.000000,   +565.500000,   +485.000000,
    +397.000031,   +302.500000,   +201.000000,   +92.500000,    -22.500015,
    -143.999985,   -272.500000,   -407.000000,   -547.500000,   -694.000000,
    -846.000000,   -1003.000000,  -1165.000000,  -1331.500000,  -1502.000000,
    -1675.500000,  -1852.500000,  -2031.500000,  -2212.500000,  -2394.000000,
    -2576.500000,  -2758.500000,  -2939.500000,  -3118.500000,  -3294.500000,
    -3467.500000,  -3635.500000,  -3798.500000,  -3955.000000,  -4104.500000,
    -4245.500000,  -4377.500000,  -4499.000000,  -4609.500000,  -4708.000000,
    -4792.500000,  -4863.500000,  -4919.000000,  -4958.000000,  -4979.500000,
    -4983.000000,  -4967.500000,  -4931.500000,  -4875.000000,  -4796.000000,
    -4694.500000,  -4569.500000,  -4420.000000,  -4246.000000,  -4046.000000,
    -3820.000000,  -3567.000000,  +3287.000000,  +2979.500000,  +2644.000000,
    +2280.500000,  +1888.000000,  +1467.500000,  +1018.500000,  +541.000000,
    +34.999992,    -498.999969,   -1061.000000,  -1650.000000,  -2266.500000,
    -2909.000000,  -3577.000000,  -4270.000000,  -4987.500000,  -5727.500000,
    -6490.000000,  -7274.000000,  -8077.500000,  -8899.500000,  -9739.000000,
    -10594.500000, -11464.500000, -12347.000000, -13241.000000, -14144.500000,
    -15056.000000, -15973.500000, -16895.500000, -17820.000000, -18744.500000,
    -19668.000000, -20588.000000, -21503.000000, -22410.500000, -23308.500000,
    -24195.000000, -25068.500000, -25926.500000, -26767.000000, -27589.000000,
    -28389.000000, -29166.500000, -29919.000000, -30644.500000, -31342.000000,
    -32009.500000, -32645.000000, -33247.000000, -33814.500000, -34346.000000,
    -34839.500000, -35295.000000, -35710.000000, -36084.500000, -36417.500000,
    -36707.500000, -36954.000000, -37156.500000, -37315.000000, -37428.000000,
    -37496.000000, +37519.000000, +37496.000000, +37428.000000, +37315.000000,
    +37156.500000, +36954.000000, +36707.500000, +36417.500000, +36084.500000,
    +35710.000000, +35295.000000, +34839.500000, +34346.000000, +33814.500000,
    +33247.000000, +32645.000000, +32009.500000, +31342.000000, +30644.500000,
    +29919.000000, +29166.500000, +28389.000000, +27589.000000, +26767.000000,
    +25926.500000, +25068.500000, +24195.000000, +23308.500000, +22410.500000,
    +21503.000000, +20588.000000, +19668.000000, +18744.500000, +17820.000000,
    +16895.500000, +15973.500000, +15056.000000, +14144.500000, +13241.000000,
    +12347.000000, +11464.500000, +10594.500000, +9739.000000,  +8899.500000,
    +8077.500000,  +7274.000000,  +6490.000000,  +5727.500000,  +4987.500000,
    +4270.000000,  +3577.000000,  +2909.000000,  +2266.500000,  +1650.000000,
    +1061.000000,  +498.999969,   -34.999992,    -541.000000,   -1018.500000,
    -1467.500000,  -1888.000000,  -2280.500000,  -2644.000000,  -2979.500000,
    +3287.000000,  +3567.000000,  +3820.000000,  +4046.000000,  +4246.000000,
    +4420.000000,  +4569.500000,  +4694.500000,  +4796.000000,  +4875.000000,
    +4931.500000,  +4967.500000,  +4983.000000,  +4979.500000,  +4958.000000,
    +4919.000000,  +4863.500000,  +4792.500000,  +4708.000000,  +4609.500000,
    +4499.000000,  +4377.500000,  +4245.500000,  +4104.500000,  +3955.000000,
    +3798.500000,  +3635.500000,  +3467.500000,  +3294.500000,  +3118.500000,
    +2939.500000,  +2758.500000,  +2576.500000,  +2394.000000,  +2212.500000,
    +2031.500000,  +1852.500000,  +1675.500000,  +1502.000000,  +1331.500000,
    +1165.000000,  +1003.000000,  +846.000000,   +694.000000,   +547.500000,
    +407.000000,   +272.500000,   +143.999985,   +22.500015,    -92.500000,
    -201.000000,   -302.500000,   -397.000031,   -485.000000,   -565.500000,
    -640.000000,   -707.000000,   -767.500000,   -822.000000,   -869.500000,
    -911.000000,   -946.500000,   -976.000000,   -1000.000000,  +1018.500000,
    +1031.500000,  +1040.000000,  +1043.500000,  +1042.500000,  +1037.500000,
    +1028.500000,  +1016.000000,  +1000.500000,  +981.000000,   +959.500000,
    +935.000000,   +908.500000,   +879.500000,   +849.000000,   +817.000000,
    +783.500000,   +749.000000,   +714.000000,   +678.000000,   +641.500000,
    +605.000000,   +568.500000,   +532.000000,   +495.500000,   +459.500000,
    +424.000000,   +389.500000,   +355.500000,   +322.500000,   +290.500000,
    +259.500031,   +229.500000,   +200.499985,   +173.500000,   +147.000000,
    +122.000015,   +98.499985,    +76.500008,    +55.500015,    +36.000008,
    +17.999987,    +1.000014,     -14.500004,    -28.500000,    -41.499985,
    -53.000011,    -63.499992,    -72.999992,    -81.500015,    -88.500008,
    -94.499992,    -100.000008,   -103.999992,   -107.500015,   -110.499985,
    -112.000008,   -113.499992,   -114.000000,   -114.000000,   -113.499992,
    -112.500015,   -110.999992,   -109.000000,   +106.500000,   +103.999992,
    +100.999985,   +98.000008,    +95.000000,    +91.499985,    +88.000008,
    +84.499985,    +80.500000,    +77.000015,    +73.500000,    +69.500008,
    +65.999992,    +62.500011,    +58.499989,    +55.500015,    +51.999996,
    +48.500015,    +45.500008,    +42.499996,    +39.499989,    +36.500011,
    +34.000011,    +31.500010,    +29.000008,    +26.500006,    +24.500011,
    +22.500015,    +20.499989,    +19.000000,    +17.500013,    +15.499985,
    +14.500004,    +13.000016,    +12.000002,    +10.500014,    +9.500000,
    +8.499987,     +8.000012,     +6.999999,     +6.499991,     +5.500010,
    +5.000004,     +4.499997,     +3.999990,     +3.500015,     +3.500015,
    +3.000009,     +2.500002,     +2.500002,     +1.999995,     +1.999995,
    +1.499988,     +1.499988,     +1.000014,     +1.000014,     +1.000014,
    +1.000014,     +0.500007,     +0.500007,     +0.500007,     +0.500007,
    +0.500007,     +0.500007
    /*
     0.000000000,  -0.000015259,  -0.000015259,  -0.000015259,  -0.000015259,
    -0.000015259,  -0.000015259,  -0.000030518,  -0.000030518,  -0.000030518,
    -0.000030518,  -0.000045776,  -0.000045776,  -0.000061035,  -0.000061035,
    -0.000076294,  -0.000076294,  -0.000091553,  -0.000106812,  -0.000106812,
    -0.000122070,  -0.000137329,  -0.000152588,  -0.000167847,  -0.000198364,
    -0.000213623,  -0.000244141,  -0.000259399,  -0.000289917,  -0.000320435,
    -0.000366211,  -0.000396729,  -0.000442505,  -0.000473022,  -0.000534058,
    -0.000579834,  -0.000625610,  -0.000686646,  -0.000747681,  -0.000808716,
    -0.000885010,  -0.000961304,  -0.001037598,  -0.001113892,  -0.001205444,
    -0.001296997,  -0.001388550,  -0.001480103,  -0.001586914,  -0.001693726,
    -0.001785278,  -0.001907349,  -0.002014160,  -0.002120972,  -0.002243042,
    -0.002349854,  -0.002456665,  -0.002578735,  -0.002685547,  -0.002792358,
    -0.002899170,  -0.002990723,  -0.003082275,  -0.003173828,   0.003250122,
     0.003326416,   0.003387451,   0.003433228,   0.003463745,   0.003479004,
     0.003479004,   0.003463745,   0.003417969,   0.003372192,   0.003280640,
     0.003173828,   0.003051758,   0.002883911,   0.002700806,   0.002487183,
     0.002227783,   0.001937866,   0.001617432,   0.001266479,   0.000869751,
     0.000442505,  -0.000030518,  -0.000549316,  -0.001098633,  -0.001693726,
    -0.002334595,  -0.003005981,  -0.003723145,  -0.004486084,  -0.005294800,
    -0.006118774,  -0.007003784,  -0.007919312,  -0.008865356,  -0.009841919,
    -0.010848999,  -0.011886597,  -0.012939453,  -0.014022827,  -0.015121460,
    -0.016235352,  -0.017349243,  -0.018463135,  -0.019577026,  -0.020690918,
    -0.021789551,  -0.022857666,  -0.023910522,  -0.024932861,  -0.025909424,
    -0.026840210,  -0.027725220,  -0.028533936,  -0.029281616,  -0.029937744,
    -0.030532837,  -0.031005859,  -0.031387329,  -0.031661987,  -0.031814575,
    -0.031845093,  -0.031738281,  -0.031478882,   0.031082153,   0.030517578,
     0.029785156,   0.028884888,   0.027801514,   0.026535034,   0.025085449,
     0.023422241,   0.021575928,   0.019531250,   0.017257690,   0.014801025,
     0.012115479,   0.009231567,   0.006134033,   0.002822876,  -0.000686646,
    -0.004394531,  -0.008316040,  -0.012420654,  -0.016708374,  -0.021179199,
    -0.025817871,  -0.030609131,  -0.035552979,  -0.040634155,  -0.045837402,
    -0.051132202,  -0.056533813,  -0.061996460,  -0.067520142,  -0.073059082,
    -0.078628540,  -0.084182739,  -0.089706421,  -0.095169067,  -0.100540161,
    -0.105819702,  -0.110946655,  -0.115921021,  -0.120697021,  -0.125259399,
    -0.129562378,  -0.133590698,  -0.137298584,  -0.140670776,  -0.143676758,
    -0.146255493,  -0.148422241,  -0.150115967,  -0.151306152,  -0.151962280,
    -0.152069092,  -0.151596069,  -0.150497437,  -0.148773193,  -0.146362305,
    -0.143264771,  -0.139450073,  -0.134887695,  -0.129577637,  -0.123474121,
    -0.116577148,  -0.108856201,   0.100311279,   0.090927124,   0.080688477,
     0.069595337,   0.057617187,   0.044784546,   0.031082153,   0.016510010,
     0.001068115,  -0.015228271,  -0.032379150,  -0.050354004,  -0.069168091,
    -0.088775635,  -0.109161377,  -0.130310059,  -0.152206421,  -0.174789429,
    -0.198059082,  -0.221984863,  -0.246505737,  -0.271591187,  -0.297210693,
    -0.323318481,  -0.349868774,  -0.376800537,  -0.404083252,  -0.431655884,
    -0.459472656,  -0.487472534,  -0.515609741,  -0.543823242,  -0.572036743,
    -0.600219727,  -0.628295898,  -0.656219482,  -0.683914185,  -0.711318970,
    -0.738372803,  -0.765029907,  -0.791213989,  -0.816864014,  -0.841949463,
    -0.866363525,  -0.890090942,  -0.913055420,  -0.935195923,  -0.956481934,
    -0.976852417,  -0.996246338,  -1.014617920,  -1.031936646,  -1.048156738,
    -1.063217163,  -1.077117920,  -1.089782715,  -1.101211548,  -1.111373901,
    -1.120223999,  -1.127746582,  -1.133926392,  -1.138763428,  -1.142211914,
    -1.144287109,   1.144989014,   1.144287109,   1.142211914,   1.138763428,
     1.133926392,   1.127746582,   1.120223999,   1.111373901,   1.101211548,
     1.089782715,   1.077117920,   1.063217163,   1.048156738,   1.031936646,
     1.014617920,   0.996246338,   0.976852417,   0.956481934,   0.935195923,
     0.913055420,   0.890090942,   0.866363525,   0.841949463,   0.816864014,
     0.791213989,   0.765029907,   0.738372803,   0.711318970,   0.683914185,
     0.656219482,   0.628295898,   0.600219727,   0.572036743,   0.543823242,
     0.515609741,   0.487472534,   0.459472656,   0.431655884,   0.404083252,
     0.376800537,   0.349868774,   0.323318481,   0.297210693,   0.271591187,
     0.246505737,   0.221984863,   0.198059082,   0.174789429,   0.152206421,
     0.130310059,   0.109161377,   0.088775635,   0.069168091,   0.050354004,
     0.032379150,   0.015228271,  -0.001068115,  -0.016510010,  -0.031082153,
    -0.044784546,  -0.057617187,  -0.069595337,  -0.080688477,  -0.090927124,
     0.100311279,   0.108856201,   0.116577148,   0.123474121,   0.129577637,
     0.134887695,   0.139450073,   0.143264771,   0.146362305,   0.148773193,
     0.150497437,   0.151596069,   0.152069092,   0.151962280,   0.151306152,
     0.150115967,   0.148422241,   0.146255493,   0.143676758,   0.140670776,
     0.137298584,   0.133590698,   0.129562378,   0.125259399,   0.120697021,
     0.115921021,   0.110946655,   0.105819702,   0.100540161,   0.095169067,
     0.089706421,   0.084182739,   0.078628540,   0.073059082,   0.067520142,
     0.061996460,   0.056533813,   0.051132202,   0.045837402,   0.040634155,
     0.035552979,   0.030609131,   0.025817871,   0.021179199,   0.016708374,
     0.012420654,   0.008316040,   0.004394531,   0.000686646,  -0.002822876,
    -0.006134033,  -0.009231567,  -0.012115479,  -0.014801025,  -0.017257690,
    -0.019531250,  -0.021575928,  -0.023422241,  -0.025085449,  -0.026535034,
    -0.027801514,  -0.028884888,  -0.029785156,  -0.030517578,   0.031082153,
     0.031478882,   0.031738281,   0.031845093,   0.031814575,   0.031661987,
     0.031387329,   0.031005859,   0.030532837,   0.029937744,   0.029281616,
     0.028533936,   0.027725220,   0.026840210,   0.025909424,   0.024932861,
     0.023910522,   0.022857666,   0.021789551,   0.020690918,   0.019577026,
     0.018463135,   0.017349243,   0.016235352,   0.015121460,   0.014022827,
     0.012939453,   0.011886597,   0.010848999,   0.009841919,   0.008865356,
     0.007919312,   0.007003784,   0.006118774,   0.005294800,   0.004486084,
     0.003723145,   0.003005981,   0.002334595,   0.001693726,   0.001098633,
     0.000549316,   0.000030518,  -0.000442505,  -0.000869751,  -0.001266479,
    -0.001617432,  -0.001937866,  -0.002227783,  -0.002487183,  -0.002700806,
    -0.002883911,  -0.003051758,  -0.003173828,  -0.003280640,  -0.003372192,
    -0.003417969,  -0.003463745,  -0.003479004,  -0.003479004,  -0.003463745,
    -0.003433228,  -0.003387451,  -0.003326416,   0.003250122,   0.003173828,
     0.003082275,   0.002990723,   0.002899170,   0.002792358,   0.002685547,
     0.002578735,   0.002456665,   0.002349854,   0.002243042,   0.002120972,
     0.002014160,   0.001907349,   0.001785278,   0.001693726,   0.001586914,
     0.001480103,   0.001388550,   0.001296997,   0.001205444,   0.001113892,
     0.001037598,   0.000961304,   0.000885010,   0.000808716,   0.000747681,
     0.000686646,   0.000625610,   0.000579834,   0.000534058,   0.000473022,
     0.000442505,   0.000396729,   0.000366211,   0.000320435,   0.000289917,
     0.000259399,   0.000244141,   0.000213623,   0.000198364,   0.000167847,
     0.000152588,   0.000137329,   0.000122070,   0.000106812,   0.000106812,
     0.000091553,   0.000076294,   0.000076294,   0.000061035,   0.000061035,
     0.000045776,   0.000045776,   0.000030518,   0.000030518,   0.000030518,
     0.000030518,   0.000015259,   0.000015259,   0.000015259,   0.000015259,
     0.000015259,   0.000015259*/
};
#endif /* SCALED_DCT */