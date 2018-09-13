%option noyywrap nodefault
%option prefix = "psu"
%{
    #include "SILibrary.h"
    #include "SIUnitParser.tab.h"
    #include "SIUnitParser.h"
%}
SYMBOL1 (EF|zsb|PSv|kGy|yph|EG|MS|csr|TSt|EH|zph|MT|dsr|yPa|mGy|GΩ|zPa|daWb|Yg|nGy|MV|EJ|as|EK|TSv|MW|Ykat|fsr|EL|at|TeV|pGy|dasb|aMx|ppq|hsr|cmol|YSt|EN|cMx|ZSt|dMx|Ym|YeV|EP|ksr|YSv|ZeV|fMx|ppt|ZSv|µGy|ES|msr|Zkat|ET|dyn|hMx|nsr|zΩ|Eha|dmol|daSt|Ys|mrad|EV|Yt|psr|EW|Gha|yGy|aSt|kMx|zGy|pA|aeV|cSt|mMx|pC|aSv|dSt|Mg|nMx|ceV|daSv|µsr|deV|cSv|fSt|pF|pMx|dSv|nrad|Mha|pG|feV|pH|hSt|fSv|Ω|Mm|ysr|pJ|heV|Pha|pK|zsr|hSv|kSt|µMx|pL|hA|keV|pN|fmol|nΩ|hC|mSt|kSv|Eg|Tha|Ms|Erad|pP|meV|nSt|Mt|yMx|Ydyn|neV|mSv|hF|pSt|zMx|dA|nSv|hG|pS|daha|hH|peV|pT|dC|Mx|pSv|Em|Yha|hJ|EBq|pV|Zha|hK|pW|prad|dF|hL|µSt|GBq|dG|Zdyn|dH|hN|µeV|fΩ|abar|µSv|Es|dJ|hP|Et|dK|ySt|dL|zSt|yeV|hS|aha|hmol|dN|daΩ|zeV|ySv|Grad|MBq|hT|aerg|zSv|cha|hV|dP|dha|hW|PBq|dS|fha|dT|pg|dV|hha|lb|TBq|dasr|dW|ph|TA|cbar|kha|ZΩ|TC|ha|pm|ℏ|mha|YBq|nha|TF|ZBq|PA|TG|TH|cerg|pha|PC|hg|TJ|ps|lm|dbar|TK|pt|PF|TL|akat|PG|PH|TN|µha|kmol|daHz|aBq|dg|hm|PJ|EHz|derg|PK|TP|cBq|PL|dBq|GHz|TS|yha|PN|TT|EWb|zha|fBq|lx|dm|hs|TV|PP|GWb|ht|TW|hBq|µrad|PS|PT|MHz|fbar|kBq|ds|PV|daPa|dt|PW|ckat|mBq|PHz|MWb|nBq|mmol|ferg|pBq|PWb|THz|dkat|TWb|Pa|Tg|nmol|µBq|Mrad|YHz|ZHz|adyn|hbar|yBq|yΩ|YWb|Pg|Tm|zBq|ZWb|Emol|herg|atm|µΩ|aHz|Pm|Ts|mmHg|Tt|cHz|dHz|kA|aWb|fkat|pmol|kC|fHz|cWb|yrad|Da|Ps|dWb|Pt|hHz|cdyn|kF|fWb|kG|kH|Ecd|mΩ|kHz|hWb|kJ|darad|damol|kK|Gmol|Gcd|mHz|Prad|kL|zrad|cA|nHz|kWb|ddyn|kN|cC|kbar|m_µ|µ_µ|ƛ_C|‰|‱|ftm|li|ch|rod|fur)
SYMBOL2 (pHz|mWb|dalm|kP|nWb|hkat|cF|Mcd|cG|kS|pWb|cH|kT|kerg|µHz|cJ|Pcd|kV|sb|cK|Hz|kW|cL|daeV|µWb|cN|aΩ|yHz|Tcd|zHz|cP|yWb|fdyn|cS|zWb|mbar|cT|dabar|Ycd|cV|Zcd|cW|merg|YΩ|sr|kg|nbar|kkat|acd|µmol|ccd|Trad|km|nerg|dcd|kn|hdyn|dacd|cd|Ebar|fcd|daph|cg|hcd|ks|kt|Elm|GA|dalx|Eerg|kcd|MΩ|GC|Glm|pbar|cm|mcd|ncd|GF|mkat|GG|pcd|Mmol|GH|dadyn|perg|GJ|cs|Mlm|Wb|GK|EOe|ct|GL|Gbar|GOe|%|zA|µcd|Plm|nkat|EΩ|GN|zC|GP|zF|Gerg|ycd|Tlm|GS|kdyn|zG|zcd|zH|GT|Elx|MOe|GV|zJ|Ekat|Glx|zK|GW|POe|zL|ymol|Ylm|Oe|Zlm|zN|zP|TOe|A|Mlx|nA|pkat|C|zS|zT|zmol|Pmol|dakat|F|Yrad|nC|Plx|H|zV|alm|St|G|YOe|zW|J|nF|K|mdyn|ZOe|L|Sv|clm|M|nG|N|nH|dlm|pΩ|Tlx|P|Gg|daA|Gkat|nJ|R|flm|nK|S|min|T|nL|Zrad|fA|V|daC|hlm|W|nN|ndyn|aOe|fC|Ylx|µbar|Gm|nP|Zlx|klm|cOe|fF|erg|dOe|daF|fG|nS|mlm|fH|nT|hΩ|nlm|b|daG|fOe|d|Gs|zg|fJ|nV|daH|Gt|fK|plm|nW|g|µerg|h|fL|Edyn|hOe|alx|fN|daJ|dΩ|kOe|m|mol|clx|Gy|zm|daK|fP|dlx|µlm|mOe|daL|ZA|s|pdyn|fS|t|flx|nOe|u|fT|Mbar|ZC|pOe|daN|hlx|ylm|zs|fV|zt|zlm|ZF|fW|Tmol|ZG|klx|daGy|ZH|daP|Merg|µOe|Gdyn|ng|ZJ|mlx|ZK|nlx|ZL|daS|µkat|plx|ZN|yOe|daT|zOe|nm|ZP|daV|ybar|ZS|µlx|ZT|TΩ|daW|fg|ns|ZV|nt|ZW|ylx|EDa|yerg|zlx|PΩ|zbar|Pbar|GDa|fm|Mkat|zerg|Perg|fs|ft|MDa|°|PDa|daOe|Zg|arad|yA|µdyn|TDa|yC|Zm|ykat|yF|µA|Ymol|yG|dag|yH|YDa|µC|ZDa|Å|Zs|yJ|Zt|yK|µF|yL|µG|zkat|Pkat|µH|yN|Zmol|crad|µJ|yP|µK|aDa|µL|Mdyn|Esb|mA|dam|Tbar|yS|rad|Eph|µN|yT|cDa|mC|daDa|Gsb|dDa|st|ly)
SYMBOL3 (EPa|µP|yV|Gph|yW|mF|fDa|drad|GPa|mG|µS|Terg|bar|mH|µT|hDa|mJ|Msb|µV|mK|µW|Mph|das|mL|kDa|MPa|dat|mN|Psb|kΩ|mDa|Pph|nDa|mP|ydyn|PPa|pDa|aA|Tsb|EGy|mS|daMx|mT|Tph|aC|GGy|yg|ua|TPa|kat|mV|frad|mW|aF|µDa|aG|Ysb|zdyn|Pdyn|Bq|aH|Zsb|cΩ|Yph|Zph|µg|ym|aJ|YPa|aK|Tkat|MGy|yDa|ZPa|Esr|aL|YA|zDa|aN|Gsr|YC|PGy|daBq|µm|ys|eV|aP|asb|yt|YF|aph|EMx|YG|aS|csb|YH|TGy|aT|aPa|daerg|dsb|cph|GMx|YJ|Msr|hrad|µs|aV|YK|cPa|µt|aW|mg|YL|dPa|fsb|fph|dph|Ybar|Psr|hsb|YN|YGy|fPa|ppb|hph|ZGy|mm|YP|MMx|hPa|ksb|Tsr|MA|ESt|YS|kph|Yerg|msb|YT|MC|kPa|nsb|EeV|GSt|mph|Zbar|PMx|ms|ESv|YV|nph|mt|mPa|GeV|YW|aGy|MF|nPa|Ysr|GSv|psb|pph|TMx|MH|Zsr|MG|cGy|amol|in|pPa|dGy|ag|MJ|Zerg|MSt|MK|Tdyn|µsb|ML|fGy|EA|MeV|µph|YMx|MN|MSv|EC|hGy|PSt|µPa|ZMx|am|MP|ppm|asr|PeV|ysb|krad|mi|yd|π|k_B|q_e|c_0|m_e|&R|E_h|ε_0|µ_0|N_A|&F|m_p|m_n|lbf|psi|oz|ton|gal|qt|cup|floz|tbsp|tsp|wk|yr|ac|cup|month|hp|°F|°C|h_P|g_0|&G|D|mD|µD|nD|µ_N|µ_B|µ_p|µ_n|µ_e|Φ_0|σ|Λ_0|b_λ|R_∞|α|g_e|g_p|g_n|a_0|floz|R_∞|lea)
UNIT {SYMBOL1}|{SYMBOL2}|{SYMBOL3}
%%
{UNIT} {
    OCStringRef string = OCStringCreateWithCString(yytext);
    psulval.unit =  SIUnitForUnderivedSymbol(string);
    OCRelease(string);
    return UNIT;
}
[+-]?[0-9]+ {
    psulval.iVal = atoi(yytext);
    return INTEGER;
}
\([+-]?[0-9]+\) {
    OCStringRef string = OCStringCreateWithCString(yytext);
    if(string) {
        OCMutableStringRef mutString = OCStringCreateMutableCopy(string);
        if(mutString) {
            OCStringTrimMatchingParentheses(mutString);
            const char *cString = OCStringGetCString(mutString);
            if(cString) {
                psulval.iVal = atoi(cString);
            }
            OCRelease(mutString);
        }
        OCRelease(string);
    }
    return INTEGER;
}
[a-zA-Z]+ {
    unitError = STR("Unknown unit symbol");
}
[\t ]+  { /* ignore whitespace */}
.      {return yytext[0];}
%%
