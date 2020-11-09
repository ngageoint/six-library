MATLAB INTERFACE

Since 2007, matlab ships with its own nitf reader.  The reader is 
adequate for some purposes, but not all.  The intent here is to externalize
all major nitf capabilities for read using NITRO's C core underneath 

The intent is to optionally support TRE extensions
(using the same TRE handlers that the C uses), as well as to support
compressed image reading (via any available NITRO decompression plugins)

Disclaimer: This interface to matlab is under development and has not been
heavily tested yet.  Testing has been done on v2007a/v2007b, and v2008b.  

You may have to tweak the configuration to get this working on
your system.  Use at your own risk.


nitf_metadata
===========================================
Function to read all the headers in a NITF:

Example usage:

If you do not set your NITF_PLUGIN_PATH and you do not pass the path as the 
second argument, your output should look as follows:

>> r = nitf_metadata(nitf)

r = 

    header: [1x1 struct]
    images: {[1x1 struct]}
>> r.header

ans = 

       FHDR: 'NITF'
       FVER: '02.10'
     CLEVEL: '03'
      STYPE: 'BF01'
     OSTAID: 'I_3005A   '
        FDT: '19970924152010'
     FTITLE: 'Checks a JPEG-compressed 64x64 8-bit mono image, lincoln's face. W/image cmts.  '
     FSCLAS: 'U'
      FSCOP: '00001'
     FSCPYS: '00001'
     ENCRYP: '0'
      ONAME: 'JITC Fort Huachuca, AZ  '
     OPHONE: '(520) 538-5458    '
         FL: '000000002407'
         HL: '000618'
       NUMI: '001'
       NUMS: '000'
       NUMX: '000'
       NUMT: '000'
     NUMDES: '000'
     NUMRES: '000'
      UDHDL: '00000'
     UDHOFL: '000'
       XHDL: '00214'
    XHDLOFL: '000'
         FS: [1x1 struct]
       UDHD: {0x1 cell}
        XHD: {[1x1 struct]}

>> r.header.XHD{1}

ans = 

        CETAG: 'JITCID'
     raw_data: [1x200 char]
    __treID__: 'raw_data'

If you add the optional second argument, the last command will yield:

>> npp = '/xdata1/u/dpressel/nitro/trunk/c/nitf/plugins/sparc-sun-solaris2.10-64';
>> r = nitf_metadata(nitf, npp);
>> r.header.XHD{1}

ans = 

        CETAG: 'JITCID'
       FILCMT: [1x200 char]
    __treID__: 'JITCID'

Notice that the TRE has one field 'FILMCT and NITRO parsed it this time.
__treID__ is not actually a part of the NITF TRE, it identifies what handler
NITRO used to parse the TRE.  This sometimes matters if a TRE has more than one
description handler (some TREs evolve over time).  NITRO tries to pick the
best handler and apply that, but sometimes its nice to know what it is

>> r.images{1}

ans = 

        IM: 'IM'
      IID1: 'Cmp_tst_01'
    IDATIM: '19970924151512'
     TGTID: '                 '
      IID2: 'This is an unclassified image in an unclassified NITF file Q1.                  '
    ISCLAS: 'U'
    ENCRYP: '0'
    ISORCE: '                                          '
     NROWS: '00000064'
     NCOLS: '00000064'
    PVTYPE: 'INT'
      IREP: 'MONO    '
      ICAT: 'VIS     '
      ABPP: '08'
     PJUST: 'R'
    ICORDS: ' '
    IGEOLO: '                                                            '
     NICOM: '9'
      ICOM: {9x1 cell}
        IC: 'C3'
    COMRAT: '00.0'
    NBANDS: '1'
    XBANDS: '00000'
     bands: {[1x1 struct]}
     ISYNC: '0'
     IMODE: 'B'
      NBPR: '0001'
      NBPC: '0001'
     NPPBH: '0064'
     NPPBV: '0064'
      NBPP: '08'
     IDLVL: '001'
     IALVL: '000'
      ILOC: '0000000000'
      IMAG: '1.0 '
     UDIDL: '00000'
     UDOFL: '000'
    IXSHDL: '00000'
    IXSOFL: '000'
        IS: [1x1 struct]
      UDID: {0x1 cell}
      XSHD: {0x1 cell}



>> i_3301 = strcat(path2, '\i_3301a.ntf');
>> out = nitf_image(i_3301, 'Window', [1, 1, 512, 512]);
>> imshow(out)