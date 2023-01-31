<?xml version="1.0" encoding="utf-8"?>
<!-- **************************************************************** -->
<!--                        UNCLASSIFIED                                                        -->
<!-- **************************************************************** -->

<!-- ****************************************************************
  INTELLIGENCE COMMUNITY TECHNICAL SPECIFICATION  
  XML DATA ENCODING SPECIFICATION FOR 
  INFORMATION SECURITY MARKING METADATA (ISM.XML)
  ****************************************************************
  Module:   IC-ISM-SecurityBanner.xsl
  Date:     2011-07-11
  Creators: Office of the Director of National Intelligence
  Intelligence Community Chief Information Officer
  **************************************************************** -->

<!-- **************************************************************** -->
<!--                            INTRODUCTION                          -->
<!--                                                                  -->
<!-- Intelligence Community Information Security Marking (IC ISM)     -->
<!-- standard was developed by the IC Security Panel for the express  -->
<!-- purpose of promoting CAPCO security marking interoperability     -->
<!-- between members of the Intelligence Community.                   -->
<!-- **************************************************************** -->

<!-- **************************************************************** -->
<!--                            DESCRIPTION                           -->
<!--                                                                  -->
<!-- This stylesheet renders a security banner marking from a         -->
<!-- document's top-level ISM attribute values. The rendered marking  -->
<!-- is compliant with the CAPCO guidelines as of the date above.     -->
<!-- **************************************************************** -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:ism="urn:us:gov:ic:ism">

  <xsl:output method="text" encoding="UTF-8" media-type="text-plain" indent="no"/>
  <!-- If including this xsl causes "Content is not allowed in prolog" the importing 
  XSL is likely missing an output declaration -->
  
  <xsl:param name="warn-missing-classif" select="'MISSING CLASSIFICATION MARKING'"/>
  <xsl:param name="warn-parse-classif" select="'UNABLE TO DETERMINE CLASSIFICATION MARKING'"/>
  <xsl:param name="warn-parse-ownerproducer"
    select="concat($warn-parse-classif, ' - MISSING OWNER/PRODUCER')"/>
  <xsl:param name="warn-parse-relto" select="'UNABLE TO DETERMINE RELEASABILITY'"/>
  <xsl:param name="warn-parse-displayonly" select="'UNABLE TO DETERMINE DISPLAY ONLY'"/>
  <xsl:param name="warn-parse-eyes" select="'UNABLE TO DETERMINE EYES ONLY MARKINGS'"/>

  <!--***********************************************-->
  <!-- replace function for 1.0 -->
  <!--***********************************************-->
  <xsl:template name="string-replace-all">
    <xsl:param name="text" />
    <xsl:param name="replace" />
    <xsl:param name="by" />
    <xsl:choose>
      <xsl:when test="contains($text, $replace)">
        <xsl:value-of select="substring-before($text,$replace)" />
        <xsl:value-of select="$by" />
        <xsl:call-template name="string-replace-all">
          <xsl:with-param name="text"
            select="substring-after($text,$replace)" />
          <xsl:with-param name="replace" select="$replace" />
          <xsl:with-param name="by" select="$by" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!--***********************************************-->
  <!-- Mode for generating the CAPCO banner -->
  <!--***********************************************-->
  <xsl:template match="*[@ism:*]" mode="ism:banner">
    <xsl:param name="portionelement"/>
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallreleaseto"/>
    <xsl:param name="documentdate" select="20090331"/>
    <xsl:call-template name="get.security.banner">
      <xsl:with-param name="portionelement" select="$portionelement"/>
      <xsl:with-param name="overalldissem" select="$overalldissem"/>
      <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
      <xsl:with-param name="documentdate" select="$documentdate"/>
    </xsl:call-template>
  </xsl:template>

  <!-- **************************************************************** -->
  <!-- Convenience template that will invoke security.banner template 
      and set the parameters with the current element's ISM attributes-->
  <!-- **************************************************************** -->
  <xsl:template name="get.security.banner">
    <xsl:param name="portionelement"/>
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallreleaseto"/>
    <xsl:param name="documentdate" select="20090331"/>
    <xsl:call-template name="security.banner">
      <xsl:with-param name="class" select="./@ism:classification"/>
      <xsl:with-param name="ownerproducer" select="./@ism:ownerProducer"/>
      <xsl:with-param name="joint" select="./@ism:joint"/>
      <xsl:with-param name="sci" select="./@ism:SCIcontrols"/>
      <xsl:with-param name="atomicenergymarkings" select="./@ism:atomicEnergyMarkings"/>
      <xsl:with-param name="sar" select="./@ism:SARIdentifier"/>
      <xsl:with-param name="fgiopen" select="./@ism:FGIsourceOpen"/>
      <xsl:with-param name="fgiprotect" select="./@ism:FGIsourceProtected"/>
      <xsl:with-param name="dissem" select="./@ism:disseminationControls"/>
      <xsl:with-param name="releaseto" select="./@ism:releasableTo"/>
      <xsl:with-param name="displayonly" select="./@ism:displayOnlyTo"/>
      <xsl:with-param name="nonic" select="./@ism:nonICmarkings"/>
      <xsl:with-param name="nonuscontrols" select="./@ism:nonUSControls"/>
      <xsl:with-param name="declassdate" select="./@ism:declassDate"/>
      <xsl:with-param name="declassexception" select="./@ism:declassException"/>
      <xsl:with-param name="declassevent" select="./@ism:declassEvent"/>
      <xsl:with-param name="typeofexemptedsource" select="./@ism:typeOfExemptedSource"/>
      <xsl:with-param name="declassmanualreview" select="./@ism:declassManualReview"/>
      <xsl:with-param name="portionelement" select="$portionelement"/>
      <xsl:with-param name="overalldissem" select="$overalldissem"/>
      <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
      <xsl:with-param name="documentdate" select="$documentdate"/>
    </xsl:call-template>
  </xsl:template>

  <!-- **************************************************************** -->
  <!--                                                                                                           -->
  <!--                     security.banner template                                         -->
  <!--                                                                                                           -->
  <!-- NOTE: The "portionelement" parameter should be specified ONLY    -->
  <!--       when the "security.banner" template is called to render a  -->
  <!--       "banner style" security marking as needed for an element   -->
  <!--       at the portion-level such as a table or figure.            -->
  <!--                                                                  -->
  <!--       The parameter can be specified in a calling stylesheet in  -->
  <!--       any of the following ways:                                 -->
  <!--                                                                  -->
  <!--       <xsl:with-param name="portionelement" select="1"/>         -->
  <!--       <xsl:with-param name="portionelement" select="'y'"/>       -->
  <!--       <xsl:with-param name="portionelement" select="'Y'"/>       -->
  <!--       <xsl:with-param name="portionelement" select="'yes'"/>     -->
  <!--       <xsl:with-param name="portionelement" select="'Yes'"/>     -->
  <!--       <xsl:with-param name="portionelement" select="'YES'"/>     -->
  <!--       <xsl:with-param name="portionelement" select="true()"/>    -->
  <!--       <xsl:with-param name="portionelement">1</xsl:with-param>   -->
  <!--       <xsl:with-param name="portionelement">y</xsl:with-param>   -->
  <!--       <xsl:with-param name="portionelement">yes</xsl:with-param> -->
  <!--       <xsl:with-param name="portionelement">Yes</xsl:with-param> -->
  <!--       <xsl:with-param name="portionelement">YES</xsl:with-param> -->
  <!--                                                                  -->
  <!-- NOTE: The "overalldissem" and "overallreleaseto" parameters are  -->
  <!--       used to compare the document-level "REL TO" or "EYES ONLY" -->
  <!--       dissemination controls to the corresponding portion-level  -->
  <!--       dissemination controls (as specified in the "dissem" and   -->
  <!--       "releaseto" parameters).                                   -->
  <!--                                                                  -->
  <!--       As per CAPCO guidelines, "REL TO" and "EYES ONLY" portion  -->
  <!--       markings can be abbreviated when they would otherwise be   -->
  <!--       identical to the corresponding document-level markings.    -->
  <!--                                                                  -->
  <!--       The "overalldissem" and "overallreleaseto" parameters are  -->
  <!--       not required.  However, if the parameters are not passed   -->
  <!--       into the template, a comparison can not be made, in which  -->
  <!--       case the full "REL TO" or "EYES ONLY" dissemination        -->
  <!--       control markings will be rendered for the portion even     -->
  <!--       when the portion-level and document-level dissemination    -->
  <!--       control markings are the same.                             -->
  <!--                                                                  -->
  <!-- **************************************************************** -->
  <xsl:template name="security.banner">
    <xsl:param name="class"/>
    <xsl:param name="ownerproducer"/>
    <xsl:param name="joint"/>
    <xsl:param name="sci"/>
    <xsl:param name="sar"/>
    <xsl:param name="atomicenergymarkings"/>
    <xsl:param name="fgiopen"/>
    <xsl:param name="fgiprotect"/>
    <xsl:param name="dissem"/>
    <xsl:param name="releaseto"/>
    <xsl:param name="displayonly"/>
    <xsl:param name="nonic"/>
    <xsl:param name="nonuscontrols"/>
    <xsl:param name="declassdate"/>
    <xsl:param name="declassexception"/>
    <xsl:param name="declassevent"/>
    <xsl:param name="typeofexemptedsource"/>
    <xsl:param name="declassmanualreview"/>
    <xsl:param name="portionelement"/>
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallreleaseto"/>
    <xsl:param name="documentdate" select="20090331"/>

    <!-- **** Normalize all of the parameters. **** -->
    <xsl:variable name="n-class" select="normalize-space($class)"/>
    <xsl:variable name="n-ownerproducer" select="normalize-space($ownerproducer)"/>
    <xsl:variable name="n-joint" select="normalize-space($joint)"/>
    <xsl:variable name="n-sci" select="normalize-space($sci)"/>
    <xsl:variable name="n-sar" select="normalize-space($sar)"/>
    <xsl:variable name="n-atomicenergymarkings" select="normalize-space($atomicenergymarkings)"/>
    <xsl:variable name="n-fgiopen" select="normalize-space($fgiopen)"/>
    <xsl:variable name="n-fgiprotect" select="normalize-space($fgiprotect)"/>
    <xsl:variable name="n-dissem">
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="normalize-space($dissem)" />
        <xsl:with-param name="replace" select="'OC OC-USGOV'" />
        <xsl:with-param name="by" select="'ORCON-USGOV'" />
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="n-releaseto" select="normalize-space($releaseto)"/>
    <xsl:variable name="n-displayonly" select="normalize-space($displayonly)"/>
    <xsl:variable name="n-nonic" select="normalize-space($nonic)"/>
    <xsl:variable name="n-nonuscontrls" select="normalize-space($nonuscontrols)"/>
    <xsl:variable name="n-declassdate" select="normalize-space($declassdate)"/>
    <xsl:variable name="n-declassexception" select="normalize-space($declassexception)"/>
    <xsl:variable name="n-declassevent" select="normalize-space($declassevent)"/>
    <xsl:variable name="n-typeofexemptedsource" select="normalize-space($typeofexemptedsource)"/>
    <xsl:variable name="n-declassmanualreview" select="normalize-space($declassmanualreview)"/>
    <xsl:variable name="n-overalldissem" select="normalize-space($overalldissem)"/>
    <xsl:variable name="n-overallreleaseto" select="normalize-space($overallreleaseto)"/>

    <xsl:variable name="isaportion">
      <xsl:choose>
        <xsl:when test="translate(normalize-space($portionelement),'YES','yes') = 'y'">1</xsl:when>
        <xsl:when test="translate(normalize-space($portionelement),'YES','yes') = 'yes'"
          >1</xsl:when>
        <xsl:when test="$portionelement = 1">1</xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="portion" select="number($isaportion)"/>

    <!-- **** Determine the classification marking **** -->
    <xsl:variable name="classVal">
      <xsl:choose>
        <xsl:when test="$n-class != ''">
          <xsl:choose>
            <xsl:when test="$n-ownerproducer = ''">
              <xsl:value-of select="$warn-parse-ownerproducer"/>
            </xsl:when>
            <xsl:when test="contains($n-ownerproducer,' ') and $n-joint='true'">
              <xsl:choose>
                <xsl:when test="$portion and $n-fgiprotect != ''">//FGI </xsl:when>
                <xsl:otherwise>
                  <xsl:if test="$n-fgiprotect = ''">
                    <xsl:text>//</xsl:text>
                      <xsl:text>JOINT </xsl:text>
                  </xsl:if>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:call-template name="ism:get.classString">
                <xsl:with-param name="source" select="$n-class"/>
              </xsl:call-template>
              <!-- <xsl:if test="not($portion and $n-fgiprotect != '')"> -->
              <xsl:if test="not($portion) and $n-fgiprotect = ''">
                <xsl:text> </xsl:text>
                <xsl:value-of select="$n-ownerproducer"/>
              </xsl:if>
            </xsl:when>
            <xsl:when test="contains($n-ownerproducer,' ') and $n-joint !='true'">
              <xsl:choose>
                <xsl:when test="$portion and $n-fgiprotect != ''">//FGI </xsl:when>
                <xsl:otherwise>
                  <xsl:if test="$n-fgiprotect = ''">
                    <xsl:text>//</xsl:text>
                  </xsl:if>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:value-of select="$n-ownerproducer"/>
              <xsl:text> </xsl:text>
              <xsl:call-template name="ism:get.classString">
                <xsl:with-param name="source" select="$n-class"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:when
              test="(($n-ownerproducer = 'USA') and not($portion and $n-fgiopen = 'UNKNOWN'))">
              <!-- **** When owner/producer is 'USA', unless this is a portion-level element and FGI source is 'UNKNOWN' **** -->
              <xsl:call-template name="ism:get.classString">
                <xsl:with-param name="source" select="$n-class"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:when test="$n-ownerproducer = 'NATO' ">
              <xsl:choose>
                <xsl:when test="$n-class='TS'">//COSMIC TOP SECRET</xsl:when>
                <xsl:when test="$n-class='S'">//NATO SECRET</xsl:when>
                <xsl:when test="$n-class='R'">//NATO RESTRICTED</xsl:when>
                <xsl:when test="$n-class='C'">//NATO CONFIDENTIAL</xsl:when>
                <xsl:when test="$n-class='U'">//NATO UNCLASSIFIED</xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$warn-parse-classif"/>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:if test="$n-nonuscontrls">
                <xsl:text>//</xsl:text>
                <xsl:value-of select="translate($n-nonuscontrls,' ','/')"/>
              </xsl:if>
            </xsl:when>
            <xsl:when test="starts-with($n-ownerproducer, 'NATO:')">
              <xsl:variable name="natoNacString">
                <xsl:call-template name="ism:get.nato.nac">
                  <xsl:with-param name="source" select="$n-ownerproducer"/>
                </xsl:call-template>
              </xsl:variable>
              <xsl:choose>
                <xsl:when test="$n-class='S'">
                  <xsl:value-of select="concat('//NATO ',$natoNacString,' SECRET')"/>
                </xsl:when>
                <xsl:when test="$n-class='R'">
                  <xsl:value-of select="concat('//NATO ',$natoNacString,' RESTRICTED')"/>
                </xsl:when>
                <xsl:when test="$n-class='C'">
                  <xsl:value-of select="concat('//NATO ',$natoNacString,' CONFIDENTIAL')"/>
                </xsl:when>
                <xsl:when test="$n-class='U'">
                  <xsl:value-of select="concat('//NATO ',$natoNacString,' UNCLASSIFIED')"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$warn-parse-classif"/>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:if test="$n-nonuscontrls">
                <xsl:text>//</xsl:text>
                <xsl:value-of select="translate($n-nonuscontrls,' ','/')"/>
              </xsl:if>
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
                <xsl:when
                  test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'R') and ($n-class != 'U')">
                  <xsl:value-of select="$warn-parse-classif"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:text>//</xsl:text>
                  <xsl:choose>
                    <xsl:when
                      test="($n-ownerproducer = 'USA' and $portion and $n-fgiopen = 'UNKNOWN') or ($portion and $n-fgiprotect != '')">
                      <xsl:text>FGI</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:value-of select="$n-ownerproducer"/>
                    </xsl:otherwise>
                  </xsl:choose>
                  <xsl:text> </xsl:text>
                  <xsl:call-template name="ism:get.classString">
                    <xsl:with-param name="source" select="$n-class"/>
                  </xsl:call-template>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$warn-missing-classif"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <!-- **** Determine the SCI marking **** -->
    <xsl:variable name="sciVal">
      <xsl:if test="$n-sci != ''">
        <xsl:text>//</xsl:text>
        <xsl:call-template name="ism:get.sci.banner">
          <xsl:with-param name="all" select="$n-sci"/>
          <xsl:with-param name="first" select="substring-before($n-sci, ' ')"/>
          <xsl:with-param name="rest" select="substring-after($n-sci, ' ')"/>
        </xsl:call-template>
        
        <xsl:if test="$n-nonuscontrls and contains($n-nonuscontrls, 'BALK')">
          <xsl:text>/BALK</xsl:text>
        </xsl:if>
        
        <xsl:if test="$n-nonuscontrls and contains($n-nonuscontrls, 'BOHEMIA')">
          <xsl:text>/BOHEMIA</xsl:text>
        </xsl:if>
        
      </xsl:if>
    </xsl:variable>

    <!-- **** Determine the SAR marking **** -->
    <xsl:variable name="sarVal">
      <xsl:if test="$n-sar != ''">
        <xsl:text>//SAR-</xsl:text>
        <xsl:choose>
          <xsl:when test="contains($n-sar,' ')">
            <xsl:call-template name="ism:get.sar.banner">
              <xsl:with-param name="first" select="substring-before($n-sar, ' ')"/>
              <xsl:with-param name="rest" select="substring-after($n-sar, ' ')"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="ism:get.sar.name">
              <xsl:with-param name="name" select="$n-sar"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </xsl:variable>


    <!-- **** Determine AtomicEnergyMarkings ****-->
    <xsl:variable name="atomicEnergyVal">
      <xsl:if test="$n-atomicenergymarkings">
        <xsl:text>//</xsl:text>

        <xsl:call-template name="ism:get.atomicEnergyMarking.banner">
          <xsl:with-param name="all" select="$n-atomicenergymarkings"/>
          <xsl:with-param name="first" select="substring-before($n-atomicenergymarkings,' ')"/>
          <xsl:with-param name="rest" select="substring-after($n-atomicenergymarkings,' ')"/>
        </xsl:call-template>
        
        <xsl:if test="$n-nonuscontrls and contains($n-nonuscontrls, 'ATOMAL')">
          <xsl:text>/ATOMAL</xsl:text>
        </xsl:if>

      </xsl:if>
    </xsl:variable>


    <!-- **** Determine the FGI marking **** -->
    <xsl:variable name="fgiVal">
      <!-- ******************************************************************************************************* -->
      <!-- FGI markings are only used when foreign government information is included in a US controlled document, -->
      <!-- or when the document is jointly controlled and 'USA' is an owner/producer and a non-US owner/producer   -->
      <!-- is protected.                                                                                           -->
      <!-- ******************************************************************************************************* -->
      <xsl:if
        test="(($n-ownerproducer = 'USA') or (contains($n-ownerproducer,'USA') and $n-fgiprotect != '')) and not($portion)">
        <xsl:choose>
          <xsl:when
            test="(($n-fgiopen != '') and (not(contains($n-fgiopen,'UNKNOWN'))) and ($n-fgiprotect = ''))">
            <xsl:text>//FGI </xsl:text>
            <xsl:value-of select="translate($n-fgiopen,':_','  ')"/>
            <xsl:if test="$n-nonuscontrls">
              <xsl:variable name="nonatocontrls">
                <xsl:value-of select="translate(
                  normalize-space(translate(translate(translate($n-nonuscontrls, 'BALK', ' '), 'BOHEMIA', ' '), 'ATOMAL', ' ')),
                  ' ','/')" />
              </xsl:variable>
              <xsl:if test="$nonatocontrls">
                <xsl:value-of select="$nonatocontrls" />
              </xsl:if>
            </xsl:if>  
          </xsl:when>
          <xsl:when test="(($n-fgiprotect != '') or (contains($n-fgiopen,'UNKNOWN')))">
            <!-- *************************************************************** -->
            <!-- Display the generic FGI marking when the document:              -->
            <!--                                                                 -->
            <!--   1.  contains some FGI from a protected source(s)              -->
            <!--   2.  contains some FGI from an unknown source(s)               -->
            <!--                                                                 -->
            <!-- *************************************************************** -->
            <xsl:text>//FGI</xsl:text>
          </xsl:when>
        </xsl:choose>
      </xsl:if>
    </xsl:variable>

    <!-- **** Determine the dissemination marking **** -->
    <xsl:variable name="dissemVal">
      <xsl:if test="$n-dissem != ''">
        <xsl:text>//</xsl:text>
        <xsl:call-template name="ism:get.dissem.banner">
          <xsl:with-param name="all" select="$n-dissem"/>
          <xsl:with-param name="first" select="substring-before($n-dissem, ' ')"/>
          <xsl:with-param name="rest" select="substring-after($n-dissem, ' ')"/>
          <xsl:with-param name="relto" select="$n-releaseto"/>
          <xsl:with-param name="displayonly" select="$n-displayonly"/>
          <xsl:with-param name="portion" select="$portion"/>
          <xsl:with-param name="overalldissem" select="$n-overalldissem"/>
          <xsl:with-param name="overallrelto" select="$n-overallreleaseto"/>
          <xsl:with-param name="ownerproducer" select="$n-ownerproducer"/>
        </xsl:call-template>
      </xsl:if>
    </xsl:variable>

    <!-- **** Determine the non-IC marking **** -->
    <xsl:variable name="nonicVal">
      <xsl:if test="$n-nonic != ''">
        <xsl:text>//</xsl:text>
        <xsl:call-template name="ism:get.nonIC.banner">
          <xsl:with-param name="all" select="$n-nonic"/>
          <xsl:with-param name="first" select="substring-before($n-nonic, ' ')"/>
          <xsl:with-param name="rest" select="substring-after($n-nonic, ' ')"/>
        </xsl:call-template>
      </xsl:if>
    </xsl:variable>

    <!-- **** Determine the declassification Manual Review marking **** -->
    <xsl:variable name="declassmanualreviewVal">
      <xsl:if test="not($portion) and ($n-class != '') and ($n-class != 'U') and ($n-class != 'NU')">
        <xsl:if
          test="(($n-declassmanualreview = 'true') or
                     ($n-typeofexemptedsource != '') or
                     (contains($n-declassexception,'25X1-human')) or
                     (contains($n-sci,'HCS')) or
                     ($n-declassevent != '') or
                     ($fgiVal != '' and $n-declassdate = '' and $n-declassexception = '') or
                     (($n-ownerproducer != '') and ($n-ownerproducer != 'USA')) or
                     (contains($n-dissem,'RD')))">
          <xsl:text>//MR</xsl:text>
        </xsl:if>
      </xsl:if>
    </xsl:variable>

    <!-- **** Determine the declassification date marking **** -->
    <xsl:variable name="declassdateVal">
      <xsl:if
        test="(not($portion) and ($n-declassexception = '') and ($declassmanualreviewVal = '') and ($n-declassdate != ''))">
        <xsl:text>//</xsl:text>
        <xsl:value-of select="substring($n-declassdate,1,4)"/>
        <xsl:variable name="month" select="substring($n-declassdate,6,2)"/>
        <xsl:choose>
          <xsl:when test="$month != ''">
            <xsl:value-of select="$month"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>01</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:variable name="day" select="substring($n-declassdate,9,2)"/>
        <xsl:choose>
          <xsl:when test="$day != ''">
            <xsl:value-of select="$day"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>01</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </xsl:variable>

    <!-- **** Determine the declassification exception marking **** -->
    <xsl:variable name="declassexceptionVal">
      <xsl:if
        test="not($portion) and ($declassmanualreviewVal = '') and ($n-declassexception != '')">
        <xsl:text>//</xsl:text>
        <xsl:choose>
          <xsl:when test="contains($n-declassexception,' ')">
            <xsl:value-of select="substring-before($n-declassexception, ' ')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$n-declassexception"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </xsl:variable>

    <!-- **** Output the values as a single string **** -->
    <xsl:value-of select="$classVal"/>
    <xsl:value-of select="$sciVal"/>
    <xsl:value-of select="$sarVal"/>
    <xsl:value-of select="$atomicEnergyVal"/>
    <xsl:value-of select="$fgiVal"/>
    <xsl:value-of select="$dissemVal"/>
    <xsl:value-of select="$nonicVal"/>

    <!-- ********************************************************************************* -->
    <!-- Note: Instead of just not including the banner declassification field, it will be -->
    <!--       included if the "document date" is earlier than 20090331.                   -->
    <!-- ********************************************************************************* -->
    <xsl:if test="number($documentdate) &lt; 20090331">
      <xsl:value-of select="$declassdateVal"/>
      <xsl:value-of select="$declassexceptionVal"/>
      <xsl:value-of select="$declassmanualreviewVal"/>

      <xsl:if
        test="(not($portion) and
                   ($n-class != '') and ($n-class != 'U') and ($n-class != 'NU') and
                   ($declassdateVal = '') and ($declassexceptionVal = '') and ($declassmanualreviewVal = ''))">
        <xsl:text>//</xsl:text>
        <xsl:value-of select="$warn-missing-classif"/>
      </xsl:if>
    </xsl:if>

  </xsl:template>

  <!-- **************************************************** -->
  <!-- A recursion routine for processing nonIC name tokens -->
  <!-- **************************************************** -->
  <xsl:template name="ism:get.nonIC.banner">
    <xsl:param name="all"/>
    <xsl:param name="first"/>
    <xsl:param name="rest"/>

    <xsl:choose>
      <xsl:when test="$first">
        <xsl:call-template name="ism:get.nonICmarking.name">
          <xsl:with-param name="name" select="$first"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="ism:get.nonICmarking.name">
          <xsl:with-param name="name" select="$all"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$rest">
      <xsl:text>/</xsl:text>
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:call-template name="ism:get.nonIC.banner">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.nonIC.banner">
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>

  </xsl:template>


  <!-- ************************************************** -->
  <!-- A recursion routine for processing atomicEnergyMarking tokens -->
  <!-- ************************************************** -->
  <xsl:template name="ism:get.atomicEnergyMarking.banner">
    <xsl:param name="all"/>
    <xsl:param name="first"/>
    <xsl:param name="rest"/>
    <xsl:param name="flagg" select="1"/>
    <xsl:choose>
      <xsl:when test="$first">
        <!-- Determine if we are dealing with a sub or sub-sub compartment and eat the early part of the token if so -->
        <xsl:choose>
          <xsl:when test="contains($first,'-')">
            <xsl:variable name="subcompartment" select="substring-after($first,'-')"/>

            <xsl:choose>
              <!-- Checks to see if there is a sub-sub such as SI-G-XXXX -->
              <xsl:when test="contains($subcompartment,'-')">

                <xsl:variable name="previous" select="substring-before($all, $first)"/>
                <xsl:variable name="sigmaTypeToken"
                  select="concat(' ', substring-before($first, concat('-',substring-after($subcompartment,'-'))))"/>
                <!--When this is the first SIGMA value for this control -->
                <xsl:if test="not(contains($previous, $sigmaTypeToken))">
                  <xsl:text>-SIGMA</xsl:text>
                </xsl:if>

                <xsl:text> </xsl:text>
                <xsl:value-of select="substring-after($subcompartment,'-')"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>-</xsl:text>
                <xsl:value-of select="$subcompartment"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <!-- Special case UCNI and DCNI when they are one of N tokens -->
          <xsl:when test="$first='DCNI'">DOD UCNI</xsl:when>
          <xsl:when test="$first='UCNI'">DOE UCNI</xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$first"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <!-- Special case UCNI and DCNI when they are the only token -->
          <xsl:when test="$all='DCNI'">DOD UCNI</xsl:when>
          <xsl:when test="$all='UCNI'">DOE UCNI</xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$all"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$rest">
      <!-- Determine if we are at the point of a slash between controls -->
      <xsl:variable name="next">
        <xsl:choose>
          <xsl:when test="contains($rest,' ')">
            <xsl:value-of select="substring-before($rest,' ')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$rest"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:if test="not(contains($next,'-'))">
        <xsl:text>/</xsl:text>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:call-template name="ism:get.atomicEnergyMarking.banner">
            <xsl:with-param name="all" select="$all"/>
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.atomicEnergyMarking.banner">
            <xsl:with-param name="all" select="$all"/>
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>

  </xsl:template>

  <!-- ************************************************** -->
  <!-- A recursion routine for processing SCI name tokens -->
  <!-- ************************************************** -->
  <xsl:template name="ism:get.sci.banner">
    <xsl:param name="all"/>
    <xsl:param name="first"/>
    <xsl:param name="rest"/>

    <xsl:choose>
      <xsl:when test="$first">
        <!-- Determine if we are dealing with a sub or sub-sub compartment and eat the early part of the token if so -->
        <xsl:choose>
          <xsl:when test="contains($first,'-')">
            <xsl:variable name="subcompartment" select="substring-after($first,'-')"/>
            <xsl:choose>
              <!-- Checks to see if there is a sub-sub such as SI-G-XXXX -->
              <xsl:when test="contains($subcompartment,'-')">
                <xsl:text> </xsl:text>
                <xsl:value-of select="substring-after($subcompartment,'-')"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>-</xsl:text>
                <xsl:value-of select="$subcompartment"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$first"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$all"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$rest">
      <!-- Determine if we are at the point of a slash between SCI controls -->
      <xsl:variable name="next">
        <xsl:choose>
          <xsl:when test="contains($rest,' ')">
            <xsl:value-of select="substring-before($rest,' ')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$rest"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:if test="not(contains($next,'-'))">
        <xsl:text>/</xsl:text>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:call-template name="ism:get.sci.banner">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.sci.banner">
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>

  </xsl:template>

  <!-- ************************************************** -->
  <!-- A recursion routine for processing SAR name tokens -->
  <!-- ************************************************** -->
  <xsl:template name="ism:get.sar.banner">
    <xsl:param name="all"/>
    <xsl:param name="first"/>
    <xsl:param name="rest"/>
    
    <xsl:choose>
      <xsl:when test="$first">
        <!-- Determine if we are dealing with a sub or sub-sub compartment and eat the early part of the token if so -->
        <xsl:choose>
          <xsl:when test="contains($first,'-')">
            <xsl:variable name="subcompartment" select="substring-after($first,'-')"/>
            <xsl:choose>
              <!-- Checks to see if there is a sub-sub such as BP-J12-J54 -->
              <xsl:when test="contains($subcompartment,'-')">
                <xsl:text> </xsl:text>
                <xsl:value-of select="substring-after($subcompartment,'-')"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>-</xsl:text>
                <xsl:value-of select="$subcompartment"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:variable name="replacedUnderscore">
              <xsl:call-template name="ism:replace"> 
                <xsl:with-param name="text" select="$first"/>
                <xsl:with-param name="find" select="'_'"/>
                <xsl:with-param name="replace" select="' '"/>
              </xsl:call-template>
            </xsl:variable>
            <xsl:value-of select="$replacedUnderscore"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="replacedUnderscore">
          <xsl:call-template name="ism:replace"> 
            <xsl:with-param name="text" select="$all"/>
            <xsl:with-param name="find" select="'_'"/>
            <xsl:with-param name="replace" select="' '"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:value-of select="$replacedUnderscore"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$rest">
      <!-- Determine if we are at the point of a slash between SAR Programs -->
      <xsl:variable name="next">
        <xsl:choose>
          <xsl:when test="contains($rest,' ')">
            <xsl:value-of select="substring-before($rest,' ')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$rest"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:if test="not(contains($next,'-'))">
        <xsl:text>/</xsl:text>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:call-template name="ism:get.sar.banner">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.sar.banner">
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    
  </xsl:template>

  <!-- ******************************************************************** -->
  <!-- A recursion routine for processing dissemination control name tokens -->
  <!-- ******************************************************************** -->
  <xsl:template name="ism:get.dissem.banner">
    <xsl:param name="all"/>
    <xsl:param name="first"/>
    <xsl:param name="rest"/>
    <xsl:param name="relto"/>
    <xsl:param name="displayonly"/>
    <xsl:param name="flagg" select="1"/>
    <xsl:param name="portion"/>
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallrelto"/>
    <xsl:param name="ownerproducer"/>
    
    <xsl:choose>
      <xsl:when test="$first">
        <xsl:if
          test="not(($first = 'RD' and (contains($rest,'RD-CNWDI') or contains($rest,'RD-SG'))) or ($first = 'FRD' and contains($rest,'FRD-SG')))">
          <xsl:choose>
            <xsl:when
              test="(starts-with($first,'RD-SG') or starts-with($first,'FRD-SG')) and $flagg &gt; 1">
              <xsl:text>SIGMA </xsl:text>
              <xsl:value-of select="substring-after($first,'SG-')"/>
            </xsl:when>
            <xsl:when test="starts-with($first,'RD-SG') and $flagg = 1">
              <xsl:text>RD-SIGMA </xsl:text>
              <xsl:value-of select="substring-after($first,'SG-')"/>
            </xsl:when>
            <xsl:when test="starts-with($first,'FRD-SG') and $flagg = 1">
              <xsl:text>FRD-SIGMA </xsl:text>
              <xsl:value-of select="substring-after($first,'SG-')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="ism:get.dissemControl.names">
                <xsl:with-param name="name" select="$first"/>
                <xsl:with-param name="rel" select="$relto"/>
                <xsl:with-param name="displayonly" select="$displayonly"/>
                <xsl:with-param name="portion" select="$portion"/>
                <xsl:with-param name="overalldissem" select="$overalldissem"/>
                <xsl:with-param name="overallrelto" select="$overallrelto"/>
                <xsl:with-param name="ownerproducer" select="$ownerproducer"/>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="starts-with($all,'RD-SG')">
            <xsl:text>RD-SIGMA </xsl:text>
            <xsl:value-of select="substring-after($all,'SG-')"/>
          </xsl:when>
          <xsl:when test="starts-with($all,'FRD-SG')">
            <xsl:text>FRD-SIGMA </xsl:text>
            <xsl:value-of select="substring-after($all,'SG-')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="ism:get.dissemControl.names">
              <xsl:with-param name="name" select="$all"/>
              <xsl:with-param name="rel" select="$relto"/>
              <xsl:with-param name="displayonly" select="$displayonly"/>
              <xsl:with-param name="portion" select="$portion"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
              <xsl:with-param name="ownerproducer" select="$ownerproducer"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$rest">
      <xsl:if
        test="not(($first = 'RD' and (contains($rest,'RD-CNWDI') or contains($rest,'RD-SG'))) or ($first = 'FRD' and contains($rest,'FRD-SG')))">
        <xsl:choose>
          <xsl:when
            test="((starts-with($first,'RD-SG') or starts-with($first,'RD-CNWDI')) and starts-with($rest,'RD-SG')) or (starts-with($first,'FRD-SG') and starts-with($rest,'FRD-SG'))">
            <xsl:text>-</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>/</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:choose>
            <xsl:when
              test="((starts-with($first,'RD-SG') or starts-with($first,'RD-CNWDI')) and starts-with($rest,'RD-SG')) or (starts-with($first,'FRD-SG') and starts-with($rest,'FRD-SG'))">
              <xsl:call-template name="ism:get.dissem.banner">
                <xsl:with-param name="first" select="substring-before($rest,' ')"/>
                <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
                <xsl:with-param name="relto" select="$relto"/>
                <xsl:with-param name="displayonly" select="$displayonly"/>
                <xsl:with-param name="flagg" select="$flagg + 1"/>
                <xsl:with-param name="portion" select="$portion"/>
                <xsl:with-param name="overalldissem" select="$overalldissem"/>
                <xsl:with-param name="overallrelto" select="$overallrelto"/>
                <xsl:with-param name="ownerproducer" select="$ownerproducer"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="ism:get.dissem.banner">
                <xsl:with-param name="first" select="substring-before($rest,' ')"/>
                <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
                <xsl:with-param name="relto" select="$relto"/>
                <xsl:with-param name="displayonly" select="$displayonly"/>
                <xsl:with-param name="portion" select="$portion"/>
                <xsl:with-param name="overalldissem" select="$overalldissem"/>
                <xsl:with-param name="overallrelto" select="$overallrelto"/>
                <xsl:with-param name="ownerproducer" select="$ownerproducer"/>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:choose>
            <xsl:when
              test="((starts-with($first,'RD-SG') or starts-with($first,'RD-CNWDI')) and starts-with($rest,'RD-SG')) or (starts-with($first,'FRD-SG') and starts-with($rest,'FRD-SG'))">
              <xsl:call-template name="ism:get.dissem.banner">
                <xsl:with-param name="first" select="$rest"/>
                <xsl:with-param name="relto" select="$relto"/>
                <xsl:with-param name="displayonly" select="$displayonly"/>
                <xsl:with-param name="flagg" select="$flagg + 1"/>
                <xsl:with-param name="portion" select="$portion"/>
                <xsl:with-param name="overalldissem" select="$overalldissem"/>
                <xsl:with-param name="overallrelto" select="$overallrelto"/>
                <xsl:with-param name="ownerproducer" select="$ownerproducer"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="ism:get.dissem.banner">
                <xsl:with-param name="first" select="$rest"/>
                <xsl:with-param name="relto" select="$relto"/>
                <xsl:with-param name="displayonly" select="$displayonly"/>
                <xsl:with-param name="portion" select="$portion"/>
                <xsl:with-param name="overalldissem" select="$overalldissem"/>
                <xsl:with-param name="overallrelto" select="$overallrelto"/>
                <xsl:with-param name="ownerproducer" select="$ownerproducer"/>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>

  </xsl:template>

  <!-- ********************************************************** -->
  <!-- Full name conversion for dissemination control name tokens -->
  <!-- and determine ReleasableTo name tokens for REL and EYES    -->
  <!-- ********************************************************** -->
  <xsl:template name="ism:get.dissemControl.names">
    <xsl:param name="name"/>
    <xsl:param name="rel"/>
    <xsl:param name="displayonly"/>
    <xsl:param name="portion"/>
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallrelto"/>
    <xsl:param name="ownerproducer"/>
    
    <xsl:choose>
      <xsl:when test="$name='RS'">RSEN</xsl:when>
      <xsl:when test="$name='OC'">ORCON</xsl:when>
      <xsl:when test="$name='IMC'">IMCON</xsl:when>
      <xsl:when test="$name='NF'">NOFORN</xsl:when>
      <xsl:when test="$name='PR'">PROPIN</xsl:when>
      <xsl:when test="$name='REL'">
        <xsl:choose>
          <xsl:when test="($rel != '') and (contains($rel,' '))">
            <xsl:choose>
              <xsl:when
                test="($portion and contains($overalldissem,'REL') and ($overallrelto = $rel))">
                <xsl:text>REL</xsl:text>
              </xsl:when>
              <xsl:otherwise>
               <xsl:choose>
                  <xsl:when test="$ownerproducer ='NATO'">
                    <xsl:text>RELEASABLE TO </xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>REL TO </xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
                
                <xsl:variable name="relString">
                  <xsl:call-template name="ism:NMTOKENS-to-CSV">
                    <xsl:with-param name="text" select="$rel"/>
                  </xsl:call-template>
                </xsl:variable>
                <xsl:value-of select="translate($relString,'_:','  ')"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$warn-parse-relto"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="$name='EYES'">
        <xsl:choose>
          <xsl:when test="($rel != '') and (contains($rel,' '))">
            <xsl:choose>
              <xsl:when
                test="($portion and contains($overalldissem,'EYES') and ($overallrelto = $rel))">
                <xsl:text>EYES</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:call-template name="ism:replace">
                  <xsl:with-param name="text" select="$rel"/>
                  <xsl:with-param name="find" select="' '"/>
                  <xsl:with-param name="replace" select="'/'"/>
                </xsl:call-template>

                <xsl:text> EYES ONLY</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$warn-parse-eyes"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="$name='LAC'">LACONIC</xsl:when>
      <xsl:when test="$name='DSEN'">DEA SENSITIVE</xsl:when>
      <xsl:when test="$name='DISPLAYONLY'">
        <xsl:text>DISPLAY ONLY </xsl:text>
        <xsl:choose>
          <xsl:when test="($displayonly != '')">
            <xsl:variable name="displayString">
              <xsl:call-template name="ism:NMTOKENS-to-CSV">
                <xsl:with-param name="text" select="$displayonly"/>
              </xsl:call-template>
            </xsl:variable>
            <xsl:value-of select="translate($displayString,'_:','  ')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$warn-parse-displayonly"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$name"/>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <!-- *************************************************** -->
  <!-- Full name conversion for a non-IC marking name token -->
  <!-- *************************************************** -->
  <xsl:template name="ism:get.nonICmarking.name">
    <xsl:param name="name"/>

    <xsl:choose>
      <xsl:when test="$name='SC'">SPECAT</xsl:when>
      <xsl:when test="$name='SINFO'">SENSITIVE INFORMATION</xsl:when>
      <xsl:when test="$name='DS'">LIMDIS</xsl:when>
      <xsl:when test="$name='XD'">EXDIS</xsl:when>
      <xsl:when test="$name='ND'">NODIS</xsl:when>
      <xsl:when test="$name='SBU-NF'">SBU NOFORN</xsl:when>
      <xsl:when test="$name='LES-NF'">LES NOFORN</xsl:when>
      <xsl:when test="starts-with($name, 'ACCM-')">
        <!-- Remove ACCM prefix from ACCM tokens -->
        <xsl:variable name="prefixlessACCM" select="substring-after($name, 'ACCM-')"/>
        <!-- Replace '_' with ' ' -->
        <xsl:value-of select="translate($prefixlessACCM, '_', ' ')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$name"/>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <!-- **************************************** -->
  <!-- Full name conversion for SAR name token -->
  <!-- **************************************** -->
  <xsl:template name="ism:get.sar.name">
    <xsl:param name="name"/>

    <!-- *********************************************************************** -->
    <!-- Set this variable to "yes" to use the program identifier abbreviations. -->
    <!-- Otherwise the program identifiers will be used.                         -->
    <!-- *********************************************************************** -->
    <xsl:variable name="abbreviate" select=" 'yes' "/>
    
    <xsl:variable name="SAR-val">
      <xsl:choose>
        <xsl:when test="substring-after($name,'SAR-')=''">
          <xsl:value-of select="concat('SAR-',$name)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="substring-after($name,'SAR-')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    
    <xsl:choose>
      <!-- ********************************************** -->
      <!-- The actual SAR program identifiers and program -->
      <!-- identifier abbreviations should be substituted -->
      <!-- for the placeholders here.                     -->
      <!-- ********************************************** -->
      <xsl:when test="$abbreviate = 'yes'">
        <xsl:value-of select="$name"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="$name='ABC'">ALPHA BRAVO CHARLIE</xsl:when>
          <xsl:when test="$name='DEF'">DELTA ECHO FOX</xsl:when>
          <xsl:when test="$name='GHI'">GULF HOTEL INDIGO</xsl:when>
          <xsl:otherwise> <!-- Name not known -->
            <xsl:value-of select="$name"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <!-- *************************************************** -->
  <!-- Tail recursion template used to replace the occurrance of one value with another -->
  <!-- *************************************************** -->
  <xsl:template name="ism:replace">
    <xsl:param name="text"/>
    <xsl:param name="find"/>
    <xsl:param name="replace"/>

    <xsl:choose>
      <xsl:when test="not(contains($text,$find))">
        <xsl:value-of select="$text"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="substring-before($text,$find)"/>
        <xsl:value-of select="$replace"/>
        <xsl:call-template name="ism:replace">
          <xsl:with-param name="text" select="substring-after($text,$find)"/>
          <xsl:with-param name="find" select="$find"/>
          <xsl:with-param name="replace" select="$replace"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- ************************************************************ -->
  <!-- Convenience template to convert space separated values (NMTOKENS) into comma separated values (CSV) -->
  <!-- ************************************************************ -->
  <xsl:template name="ism:NMTOKENS-to-CSV">
    <xsl:param name="text"/>
    <xsl:call-template name="ism:replace">
      <xsl:with-param name="text" select="$text"/>
      <xsl:with-param name="find" select="' '"/>
      <xsl:with-param name="replace" select="', '"/>
    </xsl:call-template>
  </xsl:template>
  
  <!-- ************************************************************ -->
  <!-- Get the NATO NAC string                                      -->
  <!-- ************************************************************ -->
  <xsl:template name="ism:get.nato.nac">
    <xsl:param name="source"/>
    <xsl:value-of select="translate(substring-after($source, ':'),'_',' ')"/>
  </xsl:template>


  <!-- ************************************************************ -->
  <!-- Get the Classification string                                -->
  <!-- ************************************************************ -->
  <xsl:template name="ism:get.classString">
    <xsl:param name="source"/>
    <xsl:choose>
      <xsl:when test="$source='TS'">TOP SECRET</xsl:when>
      <xsl:when test="$source='S'">SECRET</xsl:when>
      <xsl:when test="$source='C'">CONFIDENTIAL</xsl:when>
      <xsl:when test="$source='R'">RESTRICTED</xsl:when>
      <xsl:when test="$source='U'">UNCLASSIFIED</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$warn-parse-classif"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  
</xsl:stylesheet>
<!-- **************************************************************** -->
<!--                          CHANGE HISTORY                          -->
<!--                                                                  -->
<!-- Version 1.0                                                      -->
<!--                                                                  -->
<!--   - Unpublished                                                  -->
<!--                                                                  -->
<!-- Version 2.0                                                      -->
<!--                                                                  -->
<!--   - Initial published version corresponding to IC ISM v2.0       -->
<!--                                                                  -->
<!-- Version 2.0.1                                                    -->
<!--                                                                  -->
<!--   - Modified to account for new CAPCO marking requirements       -->
<!--                                                                  -->
<!--       1) New REL TO marking format                               -->
<!--       2) Dissemination controls delimiter changed from "," to    -->
<!--          "/".                                                    -->
<!--                                                                  -->
<!--   - Modified to output authorized abbreviations for all SCI      -->
<!--     controls and for the FOUO dissemination control              -->
<!--                                                                  -->
<!-- Version 2.0.2                                                    -->
<!--                                                                  -->
<!--   - Modified to correct FGI marking problems                     -->
<!--                                                                  -->
<!--       1) FGI markings are only used when foreign government      -->
<!--          information is included in a US controlled document,    -->
<!--          or when the document is jointly controlled and 'USA'    -->
<!--          is an owner/producer and a non-US owner/producer is     -->
<!--          protected.                                              -->
<!--       2) If a US controlled document or a US/non-US jointly      -->
<!--          controlled document contains any FGI from a protected   -->
<!--          source or any FGI from an unknown open source, then the -->
<!--          banner FGI marking should not include any known open    -->
<!--          FGI sources within the document.                        -->
<!--                                                                  -->
<!-- Version 2.0.3                                                    -->
<!--                                                                  -->
<!--   - Modified to account for a new authorized abbreviation for    -->
<!--     the "SPECIAL ACCESS REQUIRED" marking title.                 -->
<!--                                                                  -->
<!--   - Modified to account for the correct marking format for       -->
<!--     multiple SAR markings.  The "SAR-" prefix must be used with  -->
<!--     each SAR program identifier in security banners.             -->
<!--                                                                  -->
<!--   - Modified to account for the CAPCO guideline that the "HCS"   -->
<!--     SCI control requires the use of "MR" in the declassification -->
<!--     field of security banners.                                   -->
<!--                                                                  -->
<!--   - Modified to account for the correct marking format for       -->
<!--     multiple non-IC markings.  The delimiter is "/", not ",".    -->
<!--                                                                  -->
<!-- Version 2.0.3.1                                                  -->
<!--                                                                  -->
<!--   - Modified "get.sci.nmtokens" template to account for the      -->
<!--     correct marking format when both "SI-G" and "SI-ECI-XXX"     -->
<!--     are indicated.                                               -->
<!--                                                                  -->
<!--   - Modified "get.sci.nmtokens" template to account for the      -->
<!--     correct marking format when "SI" is indicated with either    -->
<!--     "SI-G" or "SI-ECI-XXX" or both.                              -->
<!--                                                                  -->
<!--   - Modified "get.dissem.nmtokens" template to account for the   -->
<!--     correct marking format when both "RD-CNWDI" and "RD-SG-X[X]" -->
<!--     are indicated, and when both "FRD-CNWDI" and "FRD-SG-X[X]"   -->
<!--     are indicated.                                               -->
<!--                                                                  -->
<!--   - Modified "get.dissem.nmtokens" template to account for the   -->
<!--     correct marking format when "RD" is indicated with either    -->
<!--     "RD-CNWDI" or "RD-SG-X[X]" or both, and when "FRD" is        -->
<!--     indicated with either "FRD-CNWDI" or "FRD-SG-X[X]" or both.  -->
<!--                                                                  -->
<!-- Version 2.1                                                      -->
<!--                                                                  -->
<!--   - Corresponds to IC ISM 2.1 (ISM-XML 1.0).                     -->
<!--                                                                  -->
<!--   - Modified main "security.banner" template to account for the  -->
<!--     new CAPCO guideline eliminating the declassification field   -->
<!--     from classification banner lines effective 31 March 2009     -->
<!--     (12 months after the release date of the revised CAPCO       -->
<!--     Implementation Manual and Register).                         -->
<!--                                                                  -->
<!--     Added the "documentdate" parameter with a default value of   -->
<!--     "20090331".                                                  -->
<!--                                                                  -->
<!--     Rather than simply eliminating the declassification field    -->
<!--     entirely, these modifications will allow the template to     -->
<!--     continue to include the declassification field, if the       -->
<!--     "documentdate" parameter is supplied to the template         -->
<!--      appropriately and as expected, and the value is less than   -->
<!--     "20090331" (the effective date of the new CAPCO guideline).  -->
<!--                                                                  -->
<!--     The "documentdate" parameter should be an indication of the  -->
<!--     publication date or post date of the document. The parameter -->
<!--     should be passed in as a number in YYYYMMDD format. If the   -->
<!--     parameter is not supplied to the template, or if it is       -->
<!--     supplied to the template but it is not a number (NaN), then  -->
<!--     the declassification field will not be included.             -->
<!--                                                                  -->
<!--   - Modified "get.sar.names" template to account for updated     -->
<!--     CAPCO guidelines for Special Access Program markings.        -->
<!--                                                                  -->
<!--     Previous guidelines used the terms "program identifier" and  -->
<!--     "program trigraph/digraph".  A program identifier was the    -->
<!--     full name corresponding to a program trigraph/digraph.       -->
<!--     Program identifiers were used in banners, and program        -->
<!--     trigraphs/digraphs were used in portion markings.            -->
<!--                                                                  -->
<!--     New guidelines use the terms "program identifier" and        -->
<!--     "program identifier abbreviation", and specify that either   -->
<!--     may be used in banners and that the program identifier       -->
<!--     abbreviation is used in portion markings.                    -->
<!--                                                                  -->
<!--   - Modified "get.sci.nmtokens" template to more appropriately   -->
<!--     account for non-published SCI control markings.              -->
<!--                                                                  -->
<!--   2010-05-31                                                     -->
<!--    update for ISM V4 NATO handling using new attribute           -->
<!--    nonUSControls.    -->
<!--                                                                                                             -->
<!--  Version 2010-09-24
        -  Add support for Atomic Energy Markings, Display Only, and changes to NATO classification markings.
        -  Add get.security.banner template that can be invoked when the context is the document element.  
           It will use the element's ISM attribute values to invoke the security.banner template.  
        -  Add parameters for warning message values, to allow for customization at invocation time.
        -  Namespace qualified templates, except for security.banner and get.security.banner.
        -  Moved changelog to the bottom of the file
-->
<!-- 2011-01-28                                                   
  - Added support for @ism:ACCM
  - Corrected rendering of @ism:SARIdentifier
  - Added convenience template with mode="ism:banner"
  - Changed namespace qualifier for templates to use ISM namespace, except for portionmark, get.portionmark, and get.portionmark.wxs (for legacy support).  
-->
<!-- 2011-07-11                                                   
  - Removed @ism:ACCM. ACCM values are to be expresed in @ism:nonICmarkings.
-->
<!-- 2013-02-15                                                  
  - Added logic to take the ism:joint attribute into account.
-->
<!-- 2014-06-19                                                  
  - Corrected "OC OC-USGOV" rendering for task #196.
-->
<!-- 2015-11-23
  - Corrected BALK, BOHEMIA, ATOMAL rendering for task #1100
-->
<!-- **************************************************************** -->
<!-- **************************************************************** -->
<!--                        UNCLASSIFIED                                                        -->
<!-- **************************************************************** -->
