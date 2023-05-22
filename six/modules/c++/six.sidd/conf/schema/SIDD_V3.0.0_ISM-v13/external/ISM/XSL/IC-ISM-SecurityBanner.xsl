<?xml version="1.0" encoding="utf-8"?>

<!-- **************************************************************** -->
<!--                        UNCLASSIFIED                              -->
<!-- **************************************************************** -->

<!-- ****************************************************************
  INTELLIGENCE COMMUNITY TECHNICAL SPECIFICATION  
  XML DATA ENCODING SPECIFICATION FOR 
  INFORMATION SECURITY MARKING METADATA (ISM.XML)
  ****************************************************************
  Module:   IC-ISM-SecurityBanner.xsl
  Date:     2010-05-31
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

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

<!-- **************************************************************** -->
<!--                                                                  -->
<!--                     security.banner template                     -->
<!--                                                                  -->
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
  <xsl:param name="sci"/>
  <xsl:param name="sar"/>
  <xsl:param name="fgiopen"/>
  <xsl:param name="fgiprotect"/>
  <xsl:param name="dissem"/>
  <xsl:param name="releaseto"/>
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
  <xsl:variable name="n-sci" select="normalize-space($sci)"/>
  <xsl:variable name="n-sar" select="normalize-space($sar)"/>
  <xsl:variable name="n-fgiopen" select="normalize-space($fgiopen)"/>
  <xsl:variable name="n-fgiprotect" select="normalize-space($fgiprotect)"/>
  <xsl:variable name="n-dissem" select="normalize-space($dissem)"/>
  <xsl:variable name="n-releaseto" select="normalize-space($releaseto)"/>
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
      <xsl:when test="translate(normalize-space($portionelement),'YES','yes') = 'yes'">1</xsl:when>
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
          <xsl:when test="$n-ownerproducer = ''">UNABLE TO DETERMINE CLASSIFICATION MARKING - MISSING OWNER/PRODUCER</xsl:when>
          <xsl:when test="contains($n-ownerproducer,' ')">
            <xsl:choose>
              <xsl:when test="$portion and $n-fgiprotect != ''">//FGI </xsl:when>
              <xsl:otherwise>
                <xsl:if test="$n-fgiprotect = ''">//JOINT </xsl:if>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
              <xsl:when test="$n-class='TS'">TOP SECRET</xsl:when>
              <xsl:when test="$n-class='S'">SECRET</xsl:when>
              <xsl:when test="$n-class='C'">CONFIDENTIAL</xsl:when>
              <xsl:when test="$n-class='R'">RESTRICTED</xsl:when>
              <xsl:when test="$n-class='U'">UNCLASSIFIED</xsl:when>
              <xsl:otherwise>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:otherwise>
            </xsl:choose>
            <!-- <xsl:if test="not($portion and $n-fgiprotect != '')"> -->
            <xsl:if test="not($portion) and $n-fgiprotect = ''">
              <xsl:text> </xsl:text>
              <xsl:value-of select="$n-ownerproducer"/>
            </xsl:if>
          </xsl:when>
          <xsl:when test="(($n-ownerproducer = 'USA') and not($portion and $n-fgiopen = 'UNKNOWN'))">
            <!-- **** When owner/producer is 'USA', unless this is a portion-level element and FGI source is 'UNKNOWN' **** -->
            <xsl:choose>
              <xsl:when test="$n-class='TS'">TOP SECRET</xsl:when>
              <xsl:when test="$n-class='S'">SECRET</xsl:when>
              <xsl:when test="$n-class='C'">CONFIDENTIAL</xsl:when>
              <xsl:when test="$n-class='U'">UNCLASSIFIED</xsl:when>
              <xsl:otherwise>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="($n-ownerproducer = 'NATO') and ($n-nonuscontrls = '') ">
            <xsl:choose>
              <xsl:when test="$n-class='TS'">//COSMIC TOP SECRET</xsl:when>
              <xsl:when test="$n-class='S'">//NATO SECRET</xsl:when>
              <xsl:when test="$n-class='R'">//NATO RESTRICTED</xsl:when>
              <xsl:when test="$n-class='C'">//NATO CONFIDENTIAL</xsl:when>
              <xsl:when test="$n-class='U'">//NATO UNCLASSIFIED</xsl:when>
              <xsl:otherwise>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="($n-ownerproducer = 'NATO') and not ($n-nonuscontrls = '') ">
            <xsl:choose>
              <xsl:when test="$n-class='TS'">
                <xsl:text>//COSMIC TOP SECRET</xsl:text>
                <xsl:if test="contains($n-nonuscontrls,'ATOMAL')"><xsl:text> ATOMAL</xsl:text></xsl:if>
                <xsl:if test="contains($n-nonuscontrls,'BALK')"><xsl:text>-BALK</xsl:text></xsl:if>
                <xsl:if test="contains($n-nonuscontrls,'BOHEMIA')"><xsl:text>-BOHEMIA</xsl:text></xsl:if>
              </xsl:when>
              <xsl:when test="contains($n-nonuscontrls,'ATOMAL')">
                <xsl:choose>
                  <xsl:when test="$n-class='S'">//SECRET</xsl:when>
                  <xsl:when test="$n-class='C'">//CONFIDENTIAL</xsl:when>
                  <xsl:otherwise>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:otherwise>
                </xsl:choose>
                <xsl:text> ATOMAL</xsl:text>
              </xsl:when>
              <xsl:otherwise>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'R') and ($n-class != 'U')">
                <xsl:text>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>//</xsl:text>
                <xsl:choose>
                  <xsl:when test="($n-ownerproducer = 'USA' and $portion and $n-fgiopen = 'UNKNOWN') or ($portion and $n-fgiprotect != '')">
                    <xsl:text>FGI</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:value-of select="$n-ownerproducer"/>
                  </xsl:otherwise>
                </xsl:choose>
                <xsl:text> </xsl:text>
                <xsl:choose>
                  <xsl:when test="$n-class='TS'">TOP SECRET</xsl:when>
                  <xsl:when test="$n-class='S'">SECRET</xsl:when>
                  <xsl:when test="$n-class='C'">CONFIDENTIAL</xsl:when>
                  <xsl:when test="$n-class='R'">RESTRICTED</xsl:when>
                  <xsl:when test="$n-class='U'">UNCLASSIFIED</xsl:when>
                </xsl:choose>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>MISSING CLASSIFICATION MARKING</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!-- **** Determine the SCI marking **** -->

  <xsl:variable name="sciVal">
    <xsl:if test="$n-sci != ''">
      <xsl:text>//</xsl:text>
      <xsl:call-template name="get.sci.nmtokens">
        <xsl:with-param name="all" select="$n-sci"/>
        <xsl:with-param name="first" select="substring-before($n-sci, ' ')"/>
        <xsl:with-param name="rest" select="substring-after($n-sci, ' ')"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the SAR marking **** -->

  <xsl:variable name="sarVal">
    <xsl:if test="$n-sar != ''">
      <xsl:text>//</xsl:text>
      <xsl:choose>
        <xsl:when test="contains($n-sar,' ')">
          <xsl:call-template name="get.sar.nmtokens">
            <xsl:with-param name="first" select="substring-before($n-sar, ' ')"/>
            <xsl:with-param name="rest" select="substring-after($n-sar, ' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="get.sar.names">
            <xsl:with-param name="name" select="$n-sar"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the FGI marking **** -->

  <xsl:variable name="fgiVal">
    <!-- ******************************************************************************************************* -->
    <!-- FGI markings are only used when foreign government information is included in a US controlled document, -->
    <!-- or when the document is jointly controlled and 'USA' is an owner/producer and a non-US owner/producer   -->
    <!-- is protected.                                                                                           -->
    <!-- ******************************************************************************************************* -->
    <xsl:if test="(($n-ownerproducer = 'USA') or (contains($n-ownerproducer,'USA') and $n-fgiprotect != '')) and not($portion)">
      <xsl:choose>
        <xsl:when test="(($n-fgiopen != '') and (not(contains($n-fgiopen,'UNKNOWN'))) and ($n-fgiprotect = ''))">
          <xsl:text>//FGI </xsl:text>
          <xsl:value-of select="$n-fgiopen"/>
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
      <xsl:call-template name="get.dissem.nmtokens">
        <xsl:with-param name="all" select="$n-dissem"/>
        <xsl:with-param name="first" select="substring-before($n-dissem, ' ')"/>
        <xsl:with-param name="rest" select="substring-after($n-dissem, ' ')"/>
        <xsl:with-param name="relto" select="$n-releaseto"/>
        <xsl:with-param name="portion" select="$portion"/>
        <xsl:with-param name="overalldissem" select="$n-overalldissem"/>
        <xsl:with-param name="overallrelto" select="$n-overallreleaseto"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the non-IC marking **** -->

  <xsl:variable name="nonicVal">
    <xsl:if test="$n-nonic != ''">
      <xsl:text>//</xsl:text>
      <xsl:call-template name="get.nonIC.nmtokens">
        <xsl:with-param name="all" select="$n-nonic"/>
        <xsl:with-param name="first" select="substring-before($n-nonic, ' ')"/>
        <xsl:with-param name="rest" select="substring-after($n-nonic, ' ')"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the declassification Manual Review marking **** -->

  <xsl:variable name="declassmanualreviewVal">
    <xsl:if test="not($portion) and ($n-class != '') and ($n-class != 'U') and ($n-class != 'NU')">
      <xsl:if test="(($n-declassmanualreview = 'true') or
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
    <xsl:if test="(not($portion) and ($n-declassexception = '') and ($declassmanualreviewVal = '') and ($n-declassdate != ''))">
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
    <xsl:if test="not($portion) and ($declassmanualreviewVal = '') and ($n-declassexception != '')">
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
  <xsl:if test="$sciVal != ''">
    <xsl:value-of select="$sciVal"/>
  </xsl:if>
  <xsl:if test="$sarVal != ''">
    <xsl:value-of select="$sarVal"/>
  </xsl:if>
  <xsl:value-of select="$fgiVal"/>
  <xsl:if test="$dissemVal != ''">
    <xsl:value-of select="$dissemVal"/>
  </xsl:if>
  <xsl:if test="$nonicVal != ''">
    <xsl:value-of select="$nonicVal"/>
  </xsl:if>
  <!-- ********************************************************************************* -->
  <!-- Note: Instead of just not including the banner declassification field, it will be -->
  <!--       included if the "document date" is earlier than 20090331.                   -->
  <!-- ********************************************************************************* -->
  <xsl:if test="number($documentdate) &lt; 20090331">
    <xsl:if test="$declassdateVal != ''">
      <xsl:value-of select="$declassdateVal"/>
    </xsl:if>
    <xsl:if test="$declassexceptionVal != ''">
      <xsl:value-of select="$declassexceptionVal"/>
    </xsl:if>
    <xsl:if test="$declassmanualreviewVal != ''">
      <xsl:value-of select="$declassmanualreviewVal"/>
    </xsl:if>
    <xsl:if test="(not($portion) and
                   ($n-class != '') and ($n-class != 'U') and ($n-class != 'NU') and
                   ($declassdateVal = '') and ($declassexceptionVal = '') and ($declassmanualreviewVal = ''))">
      <xsl:text>//MISSING DECLASSIFICATION MARKINGS</xsl:text>
    </xsl:if>
  </xsl:if>

</xsl:template>

<!-- **************************************************** -->
<!-- A recursion routine for processing nonIC name tokens -->
<!-- **************************************************** -->

<xsl:template name="get.nonIC.nmtokens">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:call-template name="get.nonICmarking.names">
        <xsl:with-param name="name" select="$first"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="get.nonICmarking.names">
        <xsl:with-param name="name" select="$all"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="$rest">
    <xsl:text>/</xsl:text>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="get.nonIC.nmtokens">
          <xsl:with-param name="first" select="substring-before($rest,' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get.nonIC.nmtokens">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ************************************************** -->
<!-- A recursion routine for processing SCI name tokens -->
<!-- ************************************************** -->

<xsl:template name="get.sci.nmtokens">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>
  <xsl:param name="flagg" select="1"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:if test="not($first = 'SI' and (starts-with($rest,'SI-G') or starts-with($rest,'SI-ECI')))">
        <xsl:choose>
          <xsl:when test="starts-with($first,'SI-ECI') and $flagg &gt; 1">
            <xsl:text>ECI </xsl:text>
            <xsl:value-of select="substring-after($first,'ECI-')"/>
          </xsl:when>
          <xsl:when test="starts-with($first,'SI-ECI') and $flagg = 1">
            <xsl:text>SI-ECI </xsl:text>
            <xsl:value-of select="substring-after($first,'ECI-')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$first"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
        <xsl:when test="starts-with($all,'SI-ECI')">
          <xsl:text>SI-ECI </xsl:text>
          <xsl:value-of select="substring-after($all,'ECI-')"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$all"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="$rest">
    <xsl:if test="not($first = 'SI' and (starts-with($rest,'SI-G') or starts-with($rest,'SI-ECI')))">
      <xsl:choose>
        <xsl:when test="(contains($first,'SI-ECI') or contains($first,'SI-G')) and contains($rest,'SI-ECI')">
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
          <xsl:when test="contains($rest,'SI-ECI')">
            <xsl:choose>
              <xsl:when test="contains($first,'SI-ECI') or contains($first,'SI-G')">
                <xsl:call-template name="get.sci.nmtokens">
                  <xsl:with-param name="first" select="substring-before($rest,' ')"/>
                  <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
                  <xsl:with-param name="flagg" select="$flagg + 1"/>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>
                <xsl:call-template name="get.sci.nmtokens">
                  <xsl:with-param name="first" select="substring-before($rest,' ')"/>
                  <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
                  <xsl:with-param name="flagg" select="$flagg"/>
                </xsl:call-template>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.sci.nmtokens">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="flagg" select="$flagg"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="(contains($first,'SI-ECI') or contains($first,'SI-G')) and contains($rest,'SI-ECI')">
            <xsl:call-template name="get.sci.nmtokens">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="flagg" select="$flagg + 1"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.sci.nmtokens">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="flagg" select="$flagg"/>
            </xsl:call-template>
          </xsl:otherwise> 
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ************************************************** -->
<!-- A recursion routine for processing SAR name tokens -->
<!-- ************************************************** -->

<xsl:template name="get.sar.nmtokens">
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:call-template name="get.sar.names">
    <xsl:with-param name="name" select="$first"/>
  </xsl:call-template>
  <xsl:if test="$rest">
    <xsl:text>/</xsl:text>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="get.sar.nmtokens">
          <xsl:with-param name="first" select="substring-before($rest,' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get.sar.nmtokens">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ************************************************************* -->
<!-- A recursion routine for processing EYES dissemination control -->
<!-- ************************************************************* -->

<xsl:template name="get.eyes.nmtokens">
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:value-of select="$first"/>
  <xsl:if test="$rest">
    <xsl:value-of select="'/'"/>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="get.eyes.nmtokens">
          <xsl:with-param name="first" select="substring-before($rest, ' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest, ' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get.eyes.nmtokens">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ************************************************************ -->
<!-- A recursion routine for processing REL dissemination control -->
<!-- ************************************************************ -->

<xsl:template name="get.relto.nmtokens">
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$rest">
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:value-of select="$first"/>
          <xsl:text>, </xsl:text>
          <xsl:call-template name="get.relto.nmtokens">
            <xsl:with-param name="first" select="substring-before($rest, ' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest, ' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$first"/>
          <xsl:call-template name="get.relto.nmtokens">
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>, </xsl:text>
      <xsl:value-of select="$first"/>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>

<!-- ******************************************************************** -->
<!-- A recursion routine for processing dissemination control name tokens -->
<!-- ******************************************************************** -->

<xsl:template name="get.dissem.nmtokens">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>
  <xsl:param name="relto"/>
  <xsl:param name="flagg" select="1"/>
  <xsl:param name="portion"/>
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallrelto"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:if test="not(($first = 'RD' and (contains($rest,'RD-CNWDI') or contains($rest,'RD-SG'))) or ($first = 'FRD' and (contains($rest,'FRD-CNWDI') or contains($rest,'FRD-SG'))))">
        <xsl:choose>
          <xsl:when test="(starts-with($first,'RD-SG') or starts-with($first,'FRD-SG')) and $flagg &gt; 1">
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
            <xsl:call-template name="get.dissemControl.names">
              <xsl:with-param name="name" select="$first"/>
              <xsl:with-param name="rel" select="$relto"/>
              <xsl:with-param name="portion" select="$portion"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
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
          <xsl:call-template name="get.dissemControl.names">
            <xsl:with-param name="name" select="$all"/>
            <xsl:with-param name="rel" select="$relto"/>
            <xsl:with-param name="portion" select="$portion"/>
            <xsl:with-param name="overalldissem" select="$overalldissem"/>
            <xsl:with-param name="overallrelto" select="$overallrelto"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="$rest">
    <xsl:if test="not(($first = 'RD' and (contains($rest,'RD-CNWDI') or contains($rest,'RD-SG'))) or ($first = 'FRD' and (contains($rest,'FRD-CNWDI') or contains($rest,'FRD-SG'))))">
      <xsl:choose>
        <xsl:when test="((starts-with($first,'RD-SG') or starts-with($first,'RD-CNWDI')) and starts-with($rest,'RD-SG')) or ((starts-with($first,'FRD-SG') or starts-with($first,'FRD-CNWDI')) and starts-with($rest,'FRD-SG'))">
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
          <xsl:when test="((starts-with($first,'RD-SG') or starts-with($first,'RD-CNWDI')) and starts-with($rest,'RD-SG')) or ((starts-with($first,'FRD-SG') or starts-with($first,'FRD-CNWDI')) and starts-with($rest,'FRD-SG'))">
            <xsl:call-template name="get.dissem.nmtokens">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="flagg" select="$flagg + 1"/>
              <xsl:with-param name="portion" select="$portion"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.dissem.nmtokens">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="portion" select="$portion"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="((starts-with($first,'RD-SG') or starts-with($first,'RD-CNWDI')) and starts-with($rest,'RD-SG')) or ((starts-with($first,'FRD-SG') or starts-with($first,'FRD-CNWDI')) and starts-with($rest,'FRD-SG'))">
            <xsl:call-template name="get.dissem.nmtokens">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="flagg" select="$flagg + 1"/>
              <xsl:with-param name="portion" select="$portion"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.dissem.nmtokens">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="portion" select="$portion"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
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

<xsl:template name="get.dissemControl.names">
  <xsl:param name="name"/>
  <xsl:param name="rel"/>
  <xsl:param name="portion"/>
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallrelto"/>

  <xsl:choose>
    <xsl:when test="$name='OC'">ORCON</xsl:when>
    <xsl:when test="$name='IMC'">IMCON</xsl:when>
    <xsl:when test="$name='NF'">NOFORN</xsl:when>
    <xsl:when test="$name='PR'">PROPIN</xsl:when>
    <xsl:when test="$name='REL'">
      <xsl:choose>
        <xsl:when test="($rel != '') and (contains($rel,' '))">
          <xsl:choose>
            <xsl:when test="($portion and contains($overalldissem,'REL') and ($overallrelto = $rel))">
              <xsl:text>REL</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>REL TO </xsl:text>
              <xsl:call-template name="get.relto.nmtokens">
                <xsl:with-param name="first" select="substring-before($rel,' ')"/>
                <xsl:with-param name="rest" select="substring-after($rel,' ')"/>
              </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>UNABLE TO DETERMINE RELEASABILITY</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$name='DCNI'">DOD UCNI</xsl:when>
    <xsl:when test="$name='ECNI'">DOE UCNI</xsl:when>
    <xsl:when test="$name='EYES'">
      <xsl:choose>
        <xsl:when test="($rel != '') and (contains($rel,' '))">
          <xsl:choose>
            <xsl:when test="($portion and contains($overalldissem,'EYES') and ($overallrelto = $rel))">
              <xsl:text>EYES</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="get.eyes.nmtokens">
                <xsl:with-param name="first" select="substring-before($rel,' ')"/>
                <xsl:with-param name="rest" select="substring-after($rel,' ')"/>
              </xsl:call-template>
              <xsl:text> EYES ONLY</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>UNABLE TO DETERMINE EYES ONLY MARKINGS</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$name='LAC'">LACONIC</xsl:when>
    <xsl:when test="$name='DSEN'">DEA SENSITIVE</xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$name"/>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>

<!-- *************************************************** -->
<!-- Full name conversion for non-IC marking name tokens -->
<!-- *************************************************** -->

<xsl:template name="get.nonICmarking.names">
  <xsl:param name="name"/>

  <xsl:choose>
    <xsl:when test="$name='SC'">SPECAT</xsl:when>
    <xsl:when test="$name='SIOP'">SIOP-ESI</xsl:when>
    <xsl:when test="$name='SINFO'">SENSITIVE INFORMATION</xsl:when>
    <xsl:when test="$name='DS'">LIMDIS</xsl:when>
    <xsl:when test="$name='XD'">EXDIS</xsl:when>
    <xsl:when test="$name='ND'">NODIS</xsl:when>
    <xsl:when test="$name='SBU-NF'">SBU NOFORN</xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$name"/>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>

<!-- **************************************** -->
<!-- Full name conversion for SAR name tokens -->
<!-- **************************************** -->

<xsl:template name="get.sar.names">
  <xsl:param name="name"/>

  <!-- *********************************************************************** -->
  <!-- Set this variable to "yes" to use the program identifier abbreviations. -->
  <!-- Otherwise the program identifiers will be used.                         -->
  <!-- *********************************************************************** -->
  <xsl:variable name="abbreviate" select=" 'yes' "/>

  <xsl:text>SAR-</xsl:text>
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
      </xsl:choose>
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
<!--    nonUSControls.                                                -->
<!-- **************************************************************** -->

<!-- **************************************************************** -->
<!--                        UNCLASSIFIED                              -->
<!-- **************************************************************** -->
