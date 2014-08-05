#include <vic_def.h>
#include <vic_run.h>
#include <vic_driver_image.h>

global_param_struct
get_global_param(filenames_struct *names,
                 FILE             *gp)

/**********************************************************************
   get_global_param	Keith Cherkauer	            March 1998

   This routine reads the VIC model global control file, getting
   values for global parameters, model options, and debugging controls.

   NOTE: any additions or removals of parameters in this file must also
   be made in display_current_settings.c.
**********************************************************************/
{
    extern option_struct    options;
    extern param_set_struct param_set;
    extern int              NF, NR;

    char                    cmdstr[MAXSTRING];
    char                    optstr[MAXSTRING];
    char                    flgstr[MAXSTRING];
    char                    flgstr2[MAXSTRING];
    char                    ErrStr[MAXSTRING];
    int                     file_num;
    int                     i;
    int                     tmpstartdate;
    int                     tmpenddate;
    int                     lastvalidday;
    int                     lastday[] = {
        31,     /* JANUARY */
        28,     /* FEBRUARY */
        31,     /* MARCH */
        30,     /* APRIL */
        31,     /* MAY */
        30,     /* JUNE */
        31,     /* JULY */
        31,     /* AUGUST */
        30,     /* SEPTEMBER */
        31,     /* OCTOBER */
        30,     /* NOVEMBER */
        31,     /* DECEMBER */
    };
    global_param_struct     global;

    // Initialize global parameters (that aren't part of the options struct)
    global.dt = MISSING;
    global.nrecs = MISSING;
    global.startyear = MISSING;
    global.startmonth = MISSING;
    global.startday = MISSING;
    global.starthour = MISSING;
    global.endyear = MISSING;
    global.endmonth = MISSING;
    global.endday = MISSING;
    global.resolution = MISSING;
    global.MAX_SNOW_TEMP = 0.5;
    global.MIN_RAIN_TEMP = -0.5;
    global.measure_h = 2.0;
    global.wind_h = 10.0;
    for (i = 0; i < 2; i++) {
        global.forceyear[i] = MISSING;
        global.forcemonth[i] = 1;
        global.forceday[i] = 1;
        global.forcehour[i] = 0;
        global.forceskip[i] = 0;
        strcpy(names->f_path_pfx[i], "MISSING");
    }
    file_num = 0;
    global.skipyear = 0;
    strcpy(names->init_state, "MISSING");
    global.stateyear = MISSING;
    global.statemonth = MISSING;
    global.stateday = MISSING;
    strcpy(names->statefile, "MISSING");
    strcpy(names->domain, "MISSING");
    strcpy(names->soil, "MISSING");
    strcpy(names->veg, "MISSING");
    strcpy(names->veglib, "MISSING");
    strcpy(names->snowband, "MISSING");
    strcpy(names->lakeparam, "MISSING");
    strcpy(names->result_dir, "MISSING");
    global.out_dt = MISSING;


    /** Read through global control file to find parameters **/

    rewind(gp);
    fgets(cmdstr, MAXSTRING, gp);

    while (!feof(gp)) {
        if (cmdstr[0] != '#' && cmdstr[0] != '\n' && cmdstr[0] != '\0') {
            sscanf(cmdstr, "%s", optstr);

            /* Handle case of comment line in which '#' is indented */
            if (optstr[0] == '#') {
                fgets(cmdstr, MAXSTRING, gp);
                continue;
            }

            /*************************************
               Get Model Global Parameters
            *************************************/
            if (strcasecmp("NLAYER", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &options.Nlayer);
            }
            else if (strcasecmp("NODES", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &options.Nnode);
            }
            else if (strcasecmp("TIME_STEP", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.dt);
            }
            else if (strcasecmp("SNOW_STEP", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &options.SNOW_STEP);
            }
            else if (strcasecmp("STARTYEAR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.startyear);
            }
            else if (strcasecmp("STARTMONTH", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.startmonth);
            }
            else if (strcasecmp("STARTDAY", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.startday);
            }
            else if (strcasecmp("STARTHOUR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.starthour);
            }
            else if (strcasecmp("NRECS", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.nrecs);
            }
            else if (strcasecmp("ENDYEAR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.endyear);
            }
            else if (strcasecmp("ENDMONTH", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.endmonth);
            }
            else if (strcasecmp("ENDDAY", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.endday);
            }
            else if (strcasecmp("FULL_ENERGY", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.FULL_ENERGY = TRUE;
                }
                else {
                    options.FULL_ENERGY = FALSE;
                }
            }
            else if (strcasecmp("FROZEN_SOIL", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.FROZEN_SOIL = TRUE;
                    options.QUICK_FLUX = FALSE;
                }
                else {
                    options.FROZEN_SOIL = FALSE;
                    options.IMPLICIT = FALSE;
                    options.EXP_TRANS = FALSE;
                }
            }
            else if (strcasecmp("QUICK_FLUX", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.QUICK_FLUX = TRUE;
                }
                else {
                    options.QUICK_FLUX = FALSE;
                }
            }
            else if (strcasecmp("QUICK_SOLVE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.QUICK_SOLVE = TRUE;
                }
                else {
                    options.QUICK_SOLVE = FALSE;
                }
            }
            else if ((strcasecmp("NOFLUX",
                                 optstr) == 0) ||
                     (strcasecmp("NO_FLUX", optstr) == 0)) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.NOFLUX = TRUE;
                }
                else {
                    options.NOFLUX = FALSE;
                }
            }
            else if (strcasecmp("IMPLICIT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.IMPLICIT = TRUE;
                }
                else {
                    options.IMPLICIT = FALSE;
                }
            }
            else if (strcasecmp("EXP_TRANS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.EXP_TRANS = TRUE;
                }
                else {
                    options.EXP_TRANS = FALSE;
                }
            }
            else if (strcasecmp("SNOW_DENSITY", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("DENS_SNTHRM", flgstr) == 0) {
                    options.SNOW_DENSITY = DENS_SNTHRM;
                }
                else {
                    options.SNOW_DENSITY = DENS_BRAS;
                }
            }
            else if (strcasecmp("BLOWING", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.BLOWING = TRUE;
                }
                else {
                    options.BLOWING = FALSE;
                }
            }
            else if (strcasecmp("CORRPREC", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.CORRPREC = TRUE;
                }
                else {
                    options.CORRPREC = FALSE;
                }
            }
            else if (strcasecmp("MIN_WIND_SPEED", optstr) == 0) {
                sscanf(cmdstr, "%*s %f", &options.MIN_WIND_SPEED);
            }
            else if (strcasecmp("MIN_RAIN_TEMP", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &global.MIN_RAIN_TEMP);
            }
            else if (strcasecmp("MAX_SNOW_TEMP", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &global.MAX_SNOW_TEMP);
            }
            else if (strcasecmp("CLOSE_ENERGY", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.CLOSE_ENERGY = TRUE;
                }
                else {
                    options.CLOSE_ENERGY = FALSE;
                }
            }
            else if (strcasecmp("CONTINUEONERROR", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.CONTINUEONERROR = TRUE;
                }
                else {
                    options.CONTINUEONERROR = FALSE;
                }
            }
            else if (strcasecmp("COMPUTE_TREELINE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.COMPUTE_TREELINE = FALSE;
                }
                else {
                    options.COMPUTE_TREELINE = TRUE;
                    options.AboveTreelineVeg = atoi(flgstr);
                }
            }
            else if (strcasecmp("EQUAL_AREA", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.EQUAL_AREA = TRUE;
                }
                else {
                    options.EQUAL_AREA = FALSE;
                }
            }
            else if (strcasecmp("RESOLUTION", optstr) == 0) {
                sscanf(cmdstr, "%*s %f", &global.resolution);
            }
            else if (strcasecmp("AERO_RESIST_CANSNOW", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("AR_406", flgstr) == 0) {
                    options.AERO_RESIST_CANSNOW = AR_406;
                }
                else if (strcasecmp("AR_406_LS", flgstr) == 0) {
                    options.AERO_RESIST_CANSNOW = AR_406_LS;
                }
                else if (strcasecmp("AR_406_FULL", flgstr) == 0) {
                    options.AERO_RESIST_CANSNOW = AR_406_FULL;
                }
                else if (strcasecmp("AR_410", flgstr) == 0) {
                    options.AERO_RESIST_CANSNOW = AR_410;
                }
            }
            else if (strcasecmp("GRND_FLUX_TYPE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("GF_406", flgstr) == 0) {
                    options.GRND_FLUX_TYPE = GF_406;
                }
                else if (strcasecmp("GF_410", flgstr) == 0) {
                    options.GRND_FLUX_TYPE = GF_410;
                }
            }
            else if (strcasecmp("LOG_MATRIC", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.LOG_MATRIC = TRUE;
                }
                else {
                    options.LOG_MATRIC = FALSE;
                }
            }
            else if (strcasecmp("LW_TYPE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("LW_TVA", flgstr) == 0) {
                    options.LW_TYPE = LW_TVA;
                }
                else if (strcasecmp("LW_ANDERSON", flgstr) == 0) {
                    options.LW_TYPE = LW_ANDERSON;
                }
                else if (strcasecmp("LW_BRUTSAERT", flgstr) == 0) {
                    options.LW_TYPE = LW_BRUTSAERT;
                }
                else if (strcasecmp("LW_SATTERLUND", flgstr) == 0) {
                    options.LW_TYPE = LW_SATTERLUND;
                }
                else if (strcasecmp("LW_IDSO", flgstr) == 0) {
                    options.LW_TYPE = LW_IDSO;
                }
                else if (strcasecmp("LW_PRATA", flgstr) == 0) {
                    options.LW_TYPE = LW_PRATA;
                }
            }
            else if (strcasecmp("LW_CLOUD", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("LW_CLOUD_DEARDORFF", flgstr) == 0) {
                    options.LW_CLOUD = LW_CLOUD_DEARDORFF;
                }
                else {
                    options.LW_CLOUD = LW_CLOUD_BRAS;
                }
            }
            else if (strcasecmp("MTCLIM_SWE_CORR", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.MTCLIM_SWE_CORR = TRUE;
                }
                else {
                    options.MTCLIM_SWE_CORR = FALSE;
                }
            }
            else if (strcasecmp("PLAPSE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.PLAPSE = FALSE;
                }
                else {
                    options.PLAPSE = TRUE;
                }
            }
            else if (strcasecmp("SPATIAL_FROST", optstr) == 0) {
                sscanf(cmdstr, "%*s %s %s", flgstr, flgstr2);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.SPATIAL_FROST = TRUE;
                    options.Nfrost = atoi(flgstr2);
                }
                else {
                    options.SPATIAL_FROST = FALSE;
                }
            }
            else if (strcasecmp("SPATIAL_SNOW", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.SPATIAL_SNOW = TRUE;
                }
                else {
                    options.SPATIAL_SNOW = FALSE;
                }
            }
            else if (strcasecmp("SW_PREC_THRESH", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                options.SW_PREC_THRESH = atof(flgstr);
            }
            else if (strcasecmp("TFALLBACK", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.TFALLBACK = TRUE;
                }
                else {
                    options.TFALLBACK = FALSE;
                }
            }
            else if (strcasecmp("VP_INTERP", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.VP_INTERP = TRUE;
                }
                else {
                    options.VP_INTERP = FALSE;
                }
            }
            else if (strcasecmp("VP_ITER", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("VP_ITER_NONE", flgstr) == 0) {
                    options.VP_ITER = VP_ITER_NONE;
                }
                if (strcasecmp("VP_ITER_ALWAYS", flgstr) == 0) {
                    options.VP_ITER = VP_ITER_ALWAYS;
                }
                if (strcasecmp("VP_ITER_ANNUAL", flgstr) == 0) {
                    options.VP_ITER = VP_ITER_ANNUAL;
                }
                if (strcasecmp("VP_ITER_CONVERGE", flgstr) == 0) {
                    options.VP_ITER = VP_ITER_CONVERGE;
                }
                else {
                    options.VP_INTERP = VP_ITER_ALWAYS;
                }
            }
            else if (strcasecmp("SHARE_LAYER_MOIST", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.SHARE_LAYER_MOIST = TRUE;
                }
                else {
                    options.SHARE_LAYER_MOIST = FALSE;
                }
            }
            else if (strcasecmp("CANOPY_LAYERS", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &options.Ncanopy);
            }
            else if (strcasecmp("CARBON", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.CARBON = TRUE;
                }
                else {
                    options.CARBON = FALSE;
                }
            }
            else if (strcasecmp("RC_MODE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("RC_PHOTO", flgstr) == 0) {
                    options.RC_MODE = RC_PHOTO;
                }
                else {
                    options.RC_MODE = RC_JARVIS;
                }
            }

            /*************************************
               Define state files
            *************************************/
            else if (strcasecmp("INIT_STATE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.INIT_STATE = FALSE;
                }
                else {
                    options.INIT_STATE = TRUE;
                    strcpy(names->init_state, flgstr);
                }
            }
            else if (strcasecmp("STATENAME", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->statefile);
                options.SAVE_STATE = TRUE;
            }
            else if (strcasecmp("STATEYEAR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.stateyear);
            }
            else if (strcasecmp("STATEMONTH", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.statemonth);
            }
            else if (strcasecmp("STATEDAY", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.stateday);
            }
            else if (strcasecmp("BINARY_STATE_FILE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.BINARY_STATE_FILE = FALSE;
                }
                else {
                    options.BINARY_STATE_FILE = TRUE;
                }
            }

            /*************************************
               Define forcing files
            *************************************/
            else if (strcasecmp("FORCING1", optstr) == 0) {
                if (strcmp(names->f_path_pfx[0], "MISSING") != 0) {
                    sprintf(ErrStr,
                            "Tried to define FORCING1 twice, if you want to "
                            "use two forcing files, the second must be "
                            "defined as FORCING2");
                    nrerror(ErrStr);
                }
                sscanf(cmdstr, "%*s %s", names->f_path_pfx[0]);
                file_num = 0;
            }
            else if (strcasecmp("FORCING2", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->f_path_pfx[1]);
                if (strcasecmp("FALSE", names->f_path_pfx[1]) == 0) {
                    strcpy(names->f_path_pfx[1], "MISSING");
                }
                file_num = 1;
            }
            else if (strcasecmp("FORCE_FORMAT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp(flgstr, "BINARY") == 0) {
                    param_set.FORCE_FORMAT[file_num] = BINARY;
                }
                else if (strcasecmp(flgstr, "ASCII") == 0) {
                    param_set.FORCE_FORMAT[file_num] = ASCII;
                }
                else {
                    sprintf(ErrStr,
                            "FORCE_FORMAT must be either ASCII or BINARY.");
                    nrerror(ErrStr);
                }
            }
            else if (strcasecmp("FORCE_ENDIAN", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp(flgstr, "LITTLE") == 0) {
                    param_set.FORCE_ENDIAN[file_num] = LITTLE;
                }
                else if (strcasecmp(flgstr, "BIG") == 0) {
                    param_set.FORCE_ENDIAN[file_num] = BIG;
                }
                else {
                    sprintf(ErrStr,
                            "FORCE_ENDIAN must be either BIG or LITTLE.");
                    nrerror(ErrStr);
                }
            }
            else if (strcasecmp("N_TYPES", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &param_set.N_TYPES[file_num]);
            }
            else if (strcasecmp("FORCE_DT", optstr) == 0) {
                sscanf(cmdstr, "%*s %d ", &param_set.FORCE_DT[file_num]);
            }
            else if (strcasecmp("FORCEYEAR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.forceyear[file_num]);
            }
            else if (strcasecmp("FORCEMONTH", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.forcemonth[file_num]);
            }
            else if (strcasecmp("FORCEDAY", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.forceday[file_num]);
            }
            else if (strcasecmp("FORCEHOUR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.forcehour[file_num]);
            }
            else if (strcasecmp("GRID_DECIMAL", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &options.GRID_DECIMAL);
            }
            else if (strcasecmp("WIND_H", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &global.wind_h);
            }
            else if (strcasecmp("MEASURE_H", optstr) == 0) {
                sscanf(cmdstr, "%*s %lf", &global.measure_h);
            }
            else if (strcasecmp("ALMA_INPUT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.ALMA_INPUT = TRUE;
                }
                else {
                    options.ALMA_INPUT = FALSE;
                }
            }

            /*************************************
               Define parameter files
            *************************************/

            else if (strcasecmp("DOMAIN", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->domain);
            }
            else if (strcasecmp("SOIL", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->soil);
            }
            else if (strcasecmp("ARC_SOIL", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    sprintf(ErrStr,
                            "\"ARC_SOIL\" is no longer a supported option.\n"
                            "Please convert your soil parameter file and "
                            "remove this option from your global file.");
                    nrerror(ErrStr);
                }
            }
            else if (strcasecmp("ARNO_PARAMS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    sprintf(ErrStr,
                            "Please change \"ARNO_PARAMS TRUE\" to \"BASEFLOW "
                            "NIJSSEN2001\" in your global parameter file.");
                    nrerror(ErrStr);
                }
                else {
                    sprintf(ErrStr,
                            "Please change \"ARNO_PARAMS FALSE\" to \"BASEFLOW "
                            "ARNO\" in your global parameter file.");
                    nrerror(ErrStr);
                }
            }
            else if (strcasecmp("NIJSSEN2001_BASEFLOW", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    sprintf(ErrStr,
                            "Please change \"NIJSSEN2001_BASEFLOW TRUE\" to "
                            "\"BASEFLOW NIJSSEN2001\" in your global "
                            "parameter file.");
                    nrerror(ErrStr);
                }
                else {
                    sprintf(ErrStr,
                            "Please change \"NIJSSEN2001_BASEFLOW FALSE\" to "
                            "\"BASEFLOW ARNO\" in your global parameter file.");
                    nrerror(ErrStr);
                }
            }
            else if (strcasecmp("BASEFLOW", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("NIJSSEN2001", flgstr) == 0) {
                    options.BASEFLOW = NIJSSEN2001;
                }
                else {
                    options.BASEFLOW = ARNO;
                }
            }
            else if (strcasecmp("JULY_TAVG_SUPPLIED", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.JULY_TAVG_SUPPLIED = FALSE;
                }
                else {
                    options.JULY_TAVG_SUPPLIED = TRUE;
                }
            }
            else if (strcasecmp("ORGANIC_FRACT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.ORGANIC_FRACT = FALSE;
                }
                else {
                    options.ORGANIC_FRACT = TRUE;
                }
            }
            else if (strcasecmp("VEGLIB", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->veglib);
            }
            else if (strcasecmp("VEGLIB_PHOTO", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.VEGLIB_PHOTO = TRUE;
                }
                else {
                    options.VEGLIB_PHOTO = FALSE;
                }
            }
            else if (strcasecmp("VEGPARAM", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->veg);
            }
            else if (strcasecmp("GLOBAL_LAI", optstr) == 0) {
                fprintf(stderr,
                        "WARNING: GLOBAL_LAI has been replaced by 2 new "
                        "options: VEGPARAM_LAI (whether the vegparam file "
                        "contains LAI values) and LAI_SRC (where to get LAI "
                        "values).\n");
                fprintf(stderr,
                        "\"GLOBAL_LAI  TRUE\" should now be: \"VEGPARAM_LAI "
                        "TRUE\" and \"LAI_SRC  LAI_FROM_VEGPARAM\".\n");
                fprintf(stderr,
                        "\"GLOBAL_LAI  FALSE\" should now be: \"LAI_SRC "
                        "LAI_FROM_VEGLIB\".\n");
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.VEGPARAM_LAI = TRUE;
                    options.LAI_SRC = LAI_FROM_VEGPARAM;
                }
                else {
                    options.LAI_SRC = LAI_FROM_VEGLIB;
                }
            }
            else if (strcasecmp("VEGPARAM_LAI", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.VEGPARAM_LAI = TRUE;
                }
                else {
                    options.VEGPARAM_LAI = FALSE;
                }
            }
            else if (strcasecmp("LAI_SRC", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("LAI_FROM_VEGPARAM", flgstr) == 0) {
                    options.LAI_SRC = LAI_FROM_VEGPARAM;
                }
                else {
                    options.LAI_SRC = LAI_FROM_VEGLIB;
                }
            }
            else if (strcasecmp("ROOT_ZONES", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &options.ROOT_ZONES);
            }
            else if (strcasecmp("SNOW_BAND", optstr) == 0) {
                sscanf(cmdstr, "%*s %d %s", &options.SNOW_BAND,
                       names->snowband);
            }
            else if (strcasecmp("LAKES", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.LAKES = FALSE;
                }
                else {
                    options.LAKES = TRUE;
                    strcpy(names->lakeparam, flgstr);
                }
            }
            else if (strcasecmp("LAKE_PROFILE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("FALSE", flgstr) == 0) {
                    options.LAKE_PROFILE = FALSE;
                }
                else {
                    options.LAKE_PROFILE = TRUE;
                }
            }

            /*************************************
               Define output files
            *************************************/
            else if (strcasecmp("RESULT_DIR", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", names->result_dir);
            }
            else if (strcasecmp("OUT_STEP", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.out_dt);
            }
            else if (strcasecmp("SKIPYEAR", optstr) == 0) {
                sscanf(cmdstr, "%*s %d", &global.skipyear);
            }
            else if (strcasecmp("COMPRESS", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.COMPRESS = TRUE;
                }
                else {
                    options.COMPRESS = FALSE;
                }
            }
            else if (strcasecmp("BINARY_OUTPUT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.BINARY_OUTPUT = TRUE;
                }
                else {
                    options.BINARY_OUTPUT = FALSE;
                }
            }
            else if (strcasecmp("ALMA_OUTPUT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.ALMA_OUTPUT = TRUE;
                }
                else {
                    options.ALMA_OUTPUT = FALSE;
                }
            }
            else if (strcasecmp("MOISTFRACT", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.MOISTFRACT = TRUE;
                }
                else {
                    options.MOISTFRACT = FALSE;
                }
            }
            else if (strcasecmp("OUTPUT_FORCE", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.OUTPUT_FORCE = TRUE;
                }
                else {
                    options.OUTPUT_FORCE = FALSE;
                }
            }
            else if (strcasecmp("PRT_HEADER", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.PRT_HEADER = TRUE;
                }
                else {
                    options.PRT_HEADER = FALSE;
                }
            }
            else if (strcasecmp("PRT_SNOW_BAND", optstr) == 0) {
                sscanf(cmdstr, "%*s %s", flgstr);
                if (strcasecmp("TRUE", flgstr) == 0) {
                    options.PRT_SNOW_BAND = TRUE;
                }
                else {
                    options.PRT_SNOW_BAND = FALSE;
                }
            }

            /*************************************
               Define output file contents
            *************************************/
            else if (strcasecmp("N_OUTFILES", optstr) == 0) {
                ; // do nothing
            }
            else if (strcasecmp("OUTFILE", optstr) == 0) {
                ; // do nothing
            }
            else if (strcasecmp("OUTVAR", optstr) == 0) {
                ; // do nothing
            }

            /***********************************
               Unrecognized Global Parameter Flag
            ***********************************/
            else {
                fprintf(stderr,
                        "WARNING: Unrecognized option in the global parameter "
                        "file:\n\t%s is unknown - check your spelling\n",
                        optstr);
            }
        }
        fgets(cmdstr, MAXSTRING, gp);
    }

    /******************************************
       Check for undefined required parameters
    ******************************************/

    // Validate model time step
    if (global.dt == MISSING) {
        sprintf(ErrStr,
                "Model time step has not been defined.  Make sure that the "
                "global file defines TIME_STEP.");
        nrerror(ErrStr);
    }
    else if (global.dt < 1) {
        sprintf(ErrStr,
                "The specified model time step (%d) < 1 hour.  Make sure that "
                "the global file defines a positive number of hours "
                "for TIME_STEP.",
                global.dt);
        nrerror(ErrStr);
    }

    // Validate the output step
    if (global.out_dt == 0 || global.out_dt == MISSING) {
        global.out_dt = global.dt;
    }
    else if (global.out_dt < global.dt || global.out_dt > 24 ||
             (float)global.out_dt / (float)global.dt !=
             (float)(global.out_dt / global.dt)) {
        sprintf(ErrStr,
                "Invalid output step specified. Output step must be an "
                "integer multiple of the model time step; >= model time step "
                "and <= 24");
        nrerror(ErrStr);
    }

    // Validate SNOW_STEP and set NR and NF
    if (global.dt < 24 && global.dt != options.SNOW_STEP) {
        sprintf(ErrStr,
                "If the model step is smaller than daily, the snow model "
                "should run\nat the same time step as the rest of the model.");
        nrerror(ErrStr);
    }
    if (global.dt % options.SNOW_STEP != 0 || options.SNOW_STEP > global.dt) {
        sprintf(ErrStr,
                "SNOW_STEP should be <= TIME_STEP and divide TIME_STEP "
                "evenly.");
        nrerror(ErrStr);
    }
    NF = global.dt / options.SNOW_STEP;
    if (NF == 1) {
        NR = 0;
    }
    else {
        NR = NF;
    }

    // Validate simulation start date
    if (global.startyear == MISSING) {
        sprintf(ErrStr,
                "Simulation start year has not been defined.  Make sure that "
                "the global file defines STARTYEAR.");
        nrerror(ErrStr);
    }
    else if (global.startyear < 0) {
        sprintf(ErrStr,
                "The specified simulation start year (%d) < 0.  Make sure that "
                "the global file defines a positive integer for STARTYEAR.",
                global.startyear);
        nrerror(ErrStr);
    }
    if (global.startmonth == MISSING) {
        sprintf(ErrStr,
                "Simulation start month has not been defined.  Make sure that "
                "the global file defines STARTMONTH.");
        nrerror(ErrStr);
    }
    else if (global.startmonth < 0) {
        sprintf(ErrStr,
                "The specified simulation start month (%d) < 0.  Make sure "
                "that the global file defines a positive integer for "
                "STARTMONTH.", global.startmonth);
        nrerror(ErrStr);
    }
    if (global.startday == MISSING) {
        sprintf(ErrStr,
                "Simulation start day has not been defined.  Make sure that "
                "the global file defines STARTDAY.");
        nrerror(ErrStr);
    }
    else if (global.startday < 0) {
        sprintf(ErrStr,
                "The specified simulation start day (%d) < 0.  Make sure that "
                "the global file defines a positive integer for STARTDAY.",
                global.startday);
        nrerror(ErrStr);
    }
    if (global.starthour == MISSING) {
        if (global.dt == 24) {
            global.starthour = 0;
        }
        else {
            sprintf(ErrStr,
                    "Simulation start hour has not been defined, yet model "
                    "time step is less than 24 hours.  Make sure that the "
                    "global file defines STARTHOUR.");
            nrerror(ErrStr);
        }
    }
    else if (global.starthour < 0) {
        sprintf(ErrStr,
                "The specified simulation start hour (%d) < 0.  Make sure "
                "that the global file defines a positive integer "
                "for STARTHOUR.", global.starthour);
        nrerror(ErrStr);
    }

    // Validate simulation end date and/or number of timesteps
    if (global.nrecs == MISSING && global.endyear == MISSING &&
        global.endmonth == MISSING && global.endday == MISSING) {
        sprintf(ErrStr,
                "The model global file MUST define EITHER the number of "
                "records to simulate (NRECS), or the year (ENDYEAR), month "
                "(ENDMONTH), and day (ENDDAY) of the last full simulation day");
        nrerror(ErrStr);
    }
    else if (global.nrecs == MISSING) {
        if (global.endyear == MISSING) {
            sprintf(ErrStr,
                    "Simulation end year has not been defined.  Make sure "
                    "that the global file defines ENDYEAR.");
            nrerror(ErrStr);
        }
        else if (global.endyear < 0) {
            sprintf(ErrStr,
                    "The specified simulation end year (%d) < 0.  Make sure "
                    "that the global file defines a positive integer for "
                    "ENDYEAR.", global.endyear);
            nrerror(ErrStr);
        }
        if (global.endmonth == MISSING) {
            sprintf(ErrStr,
                    "Simulation end month has not been defined.  Make sure "
                    "that the global file defines ENDMONTH.");
            nrerror(ErrStr);
        }
        else if (global.endmonth < 0) {
            sprintf(ErrStr,
                    "The specified simulation end month (%d) < 0.  Make sure "
                    "that the global file defines a positive integer for "
                    "ENDMONTH.", global.endmonth);
            nrerror(ErrStr);
        }
        if (global.endday == MISSING) {
            sprintf(ErrStr,
                    "Simulation end day has not been defined.  Make sure "
                    "that the global file defines ENDDAY.");
            nrerror(ErrStr);
        }
        else if (global.endday < 0) {
            sprintf(ErrStr,
                    "The specified simulation end day (%d) < 0.  Make sure "
                    "that the global file defines a positive integer for "
                    "ENDDAY.", global.endday);
            nrerror(ErrStr);
        }
        tmpstartdate = global.startyear * 10000 + global.startmonth * 100 +
                       global.startday;
        tmpenddate = global.endyear * 10000 + global.endmonth * 100 +
                     global.endday;
        if (tmpenddate < tmpstartdate) {
            sprintf(ErrStr,
                    "The specified simulation end date (%04d-%02d-%02d) is "
                    "EARLIER than the specified start date (%04d-%02d-%02d).",
                    global.endyear, global.endmonth, global.endday,
                    global.startyear, global.startmonth, global.startday);
            nrerror(ErrStr);
        }
    }
    else if (global.nrecs < 1) {
        sprintf(ErrStr,
                "The specified duration of simulation (%d) < 1 time step. "
                "Make sure that the global file defines a positive integer "
                "for NRECS.", global.nrecs);
        nrerror(ErrStr);
    }

    // Validate forcing files and variables
    if (strcmp(names->f_path_pfx[0], "MISSING") == 0) {
        sprintf(ErrStr,
                "No forcing file has been defined.  Make sure that the global "
                "file defines FORCING1.");
        nrerror(ErrStr);
    }
    if (param_set.N_TYPES[1] != MISSING && global.forceyear[1] == MISSING) {
        global.forceyear[1] = global.forceyear[0];
        global.forcemonth[1] = global.forcemonth[0];
        global.forceday[1] = global.forceday[0];
        global.forcehour[1] = global.forcehour[0];
        global.forceskip[1] = 0;
    }

    // Validate result directory
    if (strcmp(names->result_dir, "MISSING") == 0) {
        sprintf(ErrStr,
                "No results directory has been defined.  Make sure that the "
                "global file defines the result directory on the line that "
                "begins with \"RESULT_DIR\".");
        nrerror(ErrStr);
    }

    // Validate soil parameter file information
    if (strcmp(names->soil, "MISSING") == 0) {
        sprintf(ErrStr,
                "No soil parameter file has been defined.  Make sure that the "
                "global file defines the soil parameter file on the line that "
                "begins with \"SOIL\".");
        nrerror(ErrStr);
    }

    // Validate parameters required for normal simulations but NOT for
    // OUTPUT_FORCE

    if (!options.OUTPUT_FORCE) {
        // Validate veg parameter information
        if (strcmp(names->veg, "MISSING") == 0) {
            sprintf(ErrStr,
                    "No vegetation parameter file has been defined.  Make sure "
                    "that the global file defines the vegetation parameter "
                    "file on the line that begins with \"VEGPARAM\".");
            nrerror(ErrStr);
        }
        if (strcmp(names->veglib, "MISSING") == 0) {
            sprintf(ErrStr,
                    "No vegetation library file has been defined.  Make sure "
                    "that the global file defines the vegetation library file "
                    "on the line that begins with \"VEGLIB\".");
            nrerror(ErrStr);
        }
        if (options.ROOT_ZONES < 0) {
            sprintf(ErrStr,
                    "ROOT_ZONES must be defined to a positive integer greater "
                    "than 0, in the global control file.");
            nrerror(ErrStr);
        }
        if (options.LAI_SRC == LAI_FROM_VEGPARAM && !options.VEGPARAM_LAI) {
            sprintf(ErrStr,
                    "\"LAI_SRC\" was specified as \"LAI_FROM_VEGPARAM\", "
                    "but \"VEGPARAM_LAI\" was set to \"FALSE\" in the global "
                    "parameter file.  If you want VIC to read LAI values from "
                    "the vegparam file, you MUST make sure the veg param file "
                    "contains 1 line of 12 monthly LAI values for EACH veg "
                    "tile in EACH grid cell, and you MUST specify "
                    "\"VEGPARAM_LAI\" as \"TRUE\" in the global parameter "
                    "file.  Alternatively, if you want VIC to read LAI values "
                    "from the veg library file, set \"LAI_SRC\" to "
                    "\"LAI_FROM_VEGLIB\" in the global parameter file.  "
                    "In either case, the setting of \"VEGPARAM_LAI\" must be "
                    "consistent with the contents of the veg param file "
                    "(i.e. whether or not it contains LAI values).");
            nrerror(ErrStr);
        }

        // Validate SPATIAL_FROST information
        if (options.SPATIAL_FROST) {
            if (options.Nfrost > MAX_FROST_AREAS) {
                sprintf(ErrStr,
                        "\"SPATIAL_FROST\" was specified with %d frost "
                        "subareas, which is greater than the maximum of %d.",
                        options.Nfrost, MAX_FROST_AREAS);
                nrerror(ErrStr);
            }
            if (options.Nfrost < 1) {
                sprintf(ErrStr,
                        "\"SPATIAL_FROST\" was specified with %d frost "
                        "subareas, which is less than the mainmum of 1.",
                        options.Nfrost);
                nrerror(ErrStr);
            }
        }

        // Carbon-cycling options
        if (!options.CARBON) {
            if (options.RC_MODE == RC_PHOTO) {
                fprintf(stderr,
                        "WARNING: If CARBON==FALSE, RC_MODE must be set to "
                        "RC_JARVIS.  Setting RC_MODE to set to RC_JARVIS.\n");
                options.RC_MODE = RC_JARVIS;
            }
        }
        else {
            if (!options.VEGLIB_PHOTO) {
                sprintf(ErrStr,
                        "Currently, CARBON==TRUE and VEGLIB_PHOTO==FALSE.  "
                        "If CARBON==TRUE, VEGLIB_PHOTO must be set to TRUE and "
                        "carbon-specific veg parameters must be listed in your "
                        "veg library file.");
                nrerror(ErrStr);
            }
        }

        // Validate the elevation band file information
        if (options.SNOW_BAND > 1) {
            if (strcmp(names->snowband, "MISSING") == 0) {
                sprintf(ErrStr,
                        "\"SNOW_BAND\" was specified with %d elevation bands, "
                        "but no elevation band file has been defined.  "
                        "Make sure that the global file defines the elevation "
                        "band file on the line that begins with \"SNOW_BAND\" "
                        "(after the number of bands).", options.SNOW_BAND);
                nrerror(ErrStr);
            }
            if (options.SNOW_BAND > MAX_BANDS) {
                sprintf(ErrStr,
                        "Global file wants more snow bands (%d) than are "
                        "defined by MAX_BANDS (%d).  Edit vicNl_def.h and "
                        "recompile.", options.SNOW_BAND, MAX_BANDS);
                nrerror(ErrStr);
            }
        }
        else if (options.SNOW_BAND <= 0) {
            sprintf(ErrStr,
                    "Invalid number of elevation bands specified in global "
                    "file (%d).  Number of bands must be >= 1.",
                    options.SNOW_BAND);
            nrerror(ErrStr);
        }

        // Validate the input state file information
        if (options.INIT_STATE) {
            if (strcmp(names->init_state, "MISSING") == 0) {
                sprintf(ErrStr,
                        "\"INIT_STATE\" was specified, but no input state file "
                        "has been defined.  Make sure that the global file "
                        "defines the inputstate file on the line that begins "
                        "with \"INIT_STATE\".");
                nrerror(ErrStr);
            }
        }

        // Validate the output state file information
        if (options.SAVE_STATE) {
            if (strcmp(names->statefile, "MISSING") == 0) {
                sprintf(ErrStr,
                        "\"SAVE_STATE\" was specified, but no output state "
                        "file has been defined.  Make sure that the global "
                        "file defines the output state file on the line that "
                        "begins with \"SAVE_STATE\".");
                nrerror(ErrStr);
            }
            if (global.stateyear == MISSING || global.statemonth == MISSING ||
                global.stateday == MISSING) {
                sprintf(ErrStr,
                        "Incomplete specification of the date to save state "
                        "for state file (%s).\nSpecified date (yyyy-mm-dd): "
                        "%04d-%02d-%02d\nMake sure STATEYEAR, STATEMONTH, and "
                        "STATEDAY are set correctly in your global parameter "
                        "file.\n", names->statefile, global.stateyear,
                        global.statemonth, global.stateday);
                nrerror(ErrStr);
            }
            // Check for month, day in range
            lastvalidday = lastday[global.statemonth - 1];
            if (global.statemonth == 2) {
                if ((global.stateyear % 4) == 0 &&
                    ((global.stateyear % 100) != 0 ||
                     (global.stateyear % 400) == 0)) {
                    lastvalidday = 29;
                }
            }
            if (global.stateday > lastvalidday || global.statemonth > 12 ||
                global.statemonth < 1 || global.stateday > 31 ||
                global.stateday < 1) {
                sprintf(ErrStr,
                        "Unusual specification of the date to save state for "
                        "state file (%s).\nSpecified date (yyyy-mm-dd): "
                        "%04d-%02d-%02d\nMake sure STATEYEAR, STATEMONTH, and "
                        "STATEDAY are set correctly in your global parameter "
                        "file.\n", names->statefile, global.stateyear,
                        global.statemonth, global.stateday);
                nrerror(ErrStr);
            }
        }
        // Set the statename here to be able to compare with INIT_STATE name
        if (options.SAVE_STATE) {
            sprintf(names->statefile, "%s_%04i%02i%02i", names->statefile,
                    global.stateyear, global.statemonth, global.stateday);
        }
        if (options.INIT_STATE && options.SAVE_STATE &&
            (strcmp(names->init_state, names->statefile) == 0)) {
            sprintf(ErrStr,
                    "The save state file (%s) has the same name as the "
                    "initialize state file (%s).  The initialize state file "
                    "will be destroyed when the save state file is opened.",
                    names->statefile, names->init_state);
            nrerror(ErrStr);
        }

        // Validate soil parameter/simulation mode combinations
        if (options.QUICK_FLUX) {
            if (options.Nnode != 3) {
                fprintf(stderr,
                        "WARNING: To run the model QUICK_FLUX=TRUE, you must "
                        "define exactly 3 soil thermal nodes.  Currently "
                        "Nnodes is set to %d.  Setting Nnodes to 3.\n",
                        options.Nnode);
                options.Nnode = 3;
            }
            if (options.IMPLICIT || options.EXP_TRANS) {
                sprintf(ErrStr,
                        "To run the model with QUICK_FLUX=TRUE, you cannot "
                        "have IMPLICIT=TRUE or EXP_TRANS=TRUE.");
                nrerror(ErrStr);
            }
        }
        else {
            if (!options.FULL_ENERGY && !options.FROZEN_SOIL) {
                sprintf(ErrStr,
                        "To run the model in water balance mode (both "
                        "FULL_ENERGY and FROZEN_SOIL are FALSE) you MUST set "
                        "QUICK_FLUX to TRUE (or leave QUICK_FLUX out of your "
                        "global parameter file).");
                nrerror(ErrStr);
            }
        }
        if ((options.FULL_ENERGY ||
             options.FROZEN_SOIL) && options.Nlayer < 3) {
            sprintf(ErrStr,
                    "You must define at least 3 soil moisture layers to run "
                    "the model in FULL_ENERGY or FROZEN_SOIL modes.  "
                    "Currently Nlayers is set to  %d.", options.Nlayer);
            nrerror(ErrStr);
        }
        if ((!options.FULL_ENERGY &&
             !options.FROZEN_SOIL) && options.Nlayer < 1) {
            sprintf(ErrStr,
                    "You must define at least 1 soil moisture layer to run "
                    "the model.  Currently Nlayers is set to  %d.",
                    options.Nlayer);
            nrerror(ErrStr);
        }
        if (options.Nlayer > MAX_LAYERS) {
            sprintf(ErrStr,
                    "Global file wants more soil moisture layers (%d) than "
                    "are defined by MAX_LAYERS (%d).  Edit vicNl_def.h and "
                    "recompile.", options.Nlayer, MAX_LAYERS);
            nrerror(ErrStr);
        }
        if (options.Nnode > MAX_NODES) {
            sprintf(ErrStr,
                    "Global file wants more soil thermal nodes (%d) than "
                    "are defined by MAX_NODES (%d).  Edit vicNl_def.h and "
                    "recompile.", options.Nnode, MAX_NODES);
            nrerror(ErrStr);
        }
        if (!options.FULL_ENERGY && options.CLOSE_ENERGY) {
            sprintf(ErrStr,
                    "CLOSE_ENERGY is TRUE but FULL_ENERGY is FALSE. Set "
                    "FULL_ENERGY to TRUE to run CLOSE_ENERGY, or set "
                    "CLOSE_ENERGY to FALSE.");
            nrerror(ErrStr);
        }

        // Validate treeline option
        if (options.COMPUTE_TREELINE && !options.JULY_TAVG_SUPPLIED) {
            sprintf(ErrStr,
                    "COMPUTE_TREELINE is TRUE but JULY_TAVG_SUPPLIED is "
                    "FALSE.\n You must supply July average temperature if"
                    "you want to use the treeline option.\n");
            nrerror(ErrStr);
        }

        // Validate lake parameter information
        if (options.LAKES) {
            if (!options.FULL_ENERGY) {
                sprintf(ErrStr,
                        "FULL_ENERGY must be TRUE if the lake model is to "
                        "be run.");
                nrerror(ErrStr);
            }
            if (strcmp(names->lakeparam, "MISSING") == 0) {
                sprintf(ErrStr,
                        "\"LAKES\" was specified, but no lake parameter "
                        "file has been defined.  Make sure that the global "
                        "file defines the lake parameter file on the line that "
                        "begins with \"LAKES\".");
            }
            if (global.resolution == 0) {
                sprintf(ErrStr,
                        "The model grid cell resolution (RESOLUTION) must be "
                        "defined in the global control file when the lake "
                        "model is active.");
                nrerror(ErrStr);
            }
            if (global.resolution > 360 && !options.EQUAL_AREA) {
                sprintf(ErrStr,
                        "For EQUAL_AREA=FALSE, the model grid cell resolution "
                        "(RESOLUTION) must be set to the number of lat or lon "
                        "degrees per grid cell.  This cannot exceed 360.");
                nrerror(ErrStr);
            }
            if (options.COMPUTE_TREELINE) {
                sprintf(ErrStr,
                        "LAKES = TRUE and COMPUTE_TREELINE = TRUE are "
                        "incompatible options.");
                nrerror(ErrStr);
            }
        }

        /*********************************
           Output major options to stderr
        *********************************/
#if VERBOSE
        display_current_settings(DISP_ALL, names, &global);
#else
        display_current_settings(DISP_VERSION, names, &global);
#endif

#if VERBOSE
        fprintf(stderr, "Time Step = %d hour(s)\n", global.dt);
        fprintf(stderr, "Simulation start date = %02i/%02i/%04i\n",
                global.startday, global.startmonth, global.startyear);
        if (global.nrecs > 0) {
            fprintf(stderr, "Number of Records = %d\n\n", global.nrecs);
        }
        else {
            fprintf(stderr, "Simulation end date = %02i/%02i/%04i\n\n",
                    global.endday, global.endmonth, global.endyear);
        }
        fprintf(stderr, "Full Energy...................(%d)\n",
                options.FULL_ENERGY);
        fprintf(stderr, "Ground heat flux will be estimated ");
        if (options.QUICK_FLUX) {
            fprintf(stderr, "using Liang, Wood and Lettenmaier (1999).\n");
        }
        else {
            fprintf(stderr, "using Cherkauer and Lettenmaier (1999).\n");
        }
        fprintf(stderr, "Use Frozen Soil Model.........(%d)\n",
                options.FROZEN_SOIL);
        if (options.IMPLICIT) {
            fprintf(stderr,
                    ".... Using the implicit solution for the soil heat "
                    "equation.\n");
        }
        else {
            fprintf(stderr,
                    ".... Using the explicit solution for the soil heat "
                    "equation.\n");
        }
        if (options.EXP_TRANS) {
            fprintf(stderr,
                    ".... Thermal nodes are exponentially distributed with "
                    "depth.\n");
        }
        else {
            fprintf(stderr,
                    ".... Thermal nodes are linearly distributed with depth "
                    "(except top two nodes).\n");
        }
        fprintf(stderr, "Run Snow Model Using a Time Step of %d hours\n",
                options.SNOW_STEP);
        fprintf(stderr, "Compress Output Files.........(%d)\n",
                options.COMPRESS);
        fprintf(stderr, "Correct Precipitation.........(%d)\n",
                options.CORRPREC);
        fprintf(stderr, "\n");
        fprintf(stderr, "Using %d Snow Bands\n", options.SNOW_BAND);
        fprintf(stderr, "Using %d Root Zones\n", options.ROOT_ZONES);
        if (options.SAVE_STATE) {
            fprintf(stderr, "Model state will be saved on = %02i/%02i/%04i\n\n",
                    global.stateday, global.statemonth, global.stateyear);
        }
        if (options.BINARY_OUTPUT) {
            fprintf(stderr, "Model output is in standard BINARY format.\n");
        }
        else {
            fprintf(stderr, "Model output is in standard ASCII format.\n");
        }

#endif // VERBOSE
    } // !OUTPUT_FORCE

    return global;
}
