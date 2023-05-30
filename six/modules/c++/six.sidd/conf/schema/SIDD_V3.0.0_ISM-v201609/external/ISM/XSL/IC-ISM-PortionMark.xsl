<?xml version="1.0" encoding="utf-8"?>
<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->
<!-- ****************************************************************
  INTELLIGENCE COMMUNITY TECHNICAL SPECIFICATION  
  XML DATA ENCODING SPECIFICATION FOR 
  INFORMATION SECURITY MARKING METADATA (ISM.XML)
  ****************************************************************
  Module:   IC-ISM-PortionMark.xsl
  Date:     2010-05-31
  Creators: Office of the Director of National Intelligence
  Intelligence Community Chief Information Officer
  **************************************************************** -->
<!-- **************************************************************** -->
<!--                            INTRODUCTION                          -->
<!-- **************************************************************** -->
<!-- This XSLT file is one component of the ISM.XML Data Encoding 
  Specification (DES). Please see the document titled 
  XML DATA ENCODING SPECIFICATION FOR INFORMATION SECURITY MARKING METADATA 
  for a complete description of the encoding as well as list
  of all components.  
  
  It is envisioned that this XSLT or its components, as well as other
  parts of the DES may be overridden for localized implementations. 
  Therefore, permission to use, copy, modify and distribute this XSLT
  and the other parts of the DES for any purpose is hereby 
  granted in perpetuity.
  
  Please reference the preceding two paragraphs in all copies or
  variations. The developers make no representation about the
  suitability of the schema or DES for any purpose. It is provided 
  "as is" without expressed or implied warranty.
  
  If you modify this XSLT in any way label it
  as a variant of ISM.XML. 
  
  Please direct all questions, bug reports,or suggestions for changes
  to the points of contact identified in the document referenced above.
-->
<!-- **************************************************************** -->
<!--                            DESCRIPTION                           -->
<!--                                                                  -->
<!-- This stylesheet renders a portion marking from the ISM attribute -->
<!-- values of a portion-level element.  The rendered marking is      -->
<!-- compliant with the CAPCO guidelines as of the date above.        -->
<!-- **************************************************************** -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:ism="urn:us:gov:ic:ism"
                version="1.0">

<!-- **************************************************************** -->
<!-- portionmark - renders the security portion marking for each of   -->
<!--               the document's portion level elements.             -->
<!-- **************************************************************** -->

<!-- **************************************************************** -->
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
<!-- **************************************************************** -->

<xsl:template name="portionmark">
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
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallreleaseto"/>

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
  <xsl:variable name="n-overalldissem" select="normalize-space($overalldissem)"/>
  <xsl:variable name="n-overallreleaseto" select="normalize-space($overallreleaseto)"/>

  <!-- **** Determine the classification marking **** -->
  <xsl:variable name="classVal">
    <xsl:choose>
      <xsl:when test="$n-class != ''">
        <xsl:choose>
          <xsl:when test="$n-ownerproducer = ''">UNABLE TO DETERMINE CLASSIFICATION MARKING - MISSING OWNER/PRODUCER</xsl:when>
          <xsl:when test="contains($n-ownerproducer,' ') and ($n-fgiprotect = '')">
            <xsl:choose>
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'R') and ($n-class != 'U')">
                <xsl:text>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>//JOINT </xsl:text>
                <xsl:value-of select="$n-class"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$n-ownerproducer"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="($n-ownerproducer = 'USA') and ($n-fgiopen != 'UNKNOWN') and ($n-fgiprotect = '')">
            <xsl:choose>
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'U')">
                <xsl:text>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$n-class"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="$n-ownerproducer = 'NATO'">
            <xsl:choose>
              <xsl:when test="$n-class = 'TS'">
                <xsl:text>//CTS</xsl:text>
                <xsl:if test="contains($n-nonuscontrls,'ATOMAL')"><xsl:text>A</xsl:text></xsl:if>
                <xsl:if test="contains($n-nonuscontrls,'BALK')"><xsl:text>-BALK</xsl:text></xsl:if>
                <xsl:if test="contains($n-nonuscontrls,'BOHEMIA')"><xsl:text>-B</xsl:text></xsl:if>
              </xsl:when>
              <xsl:when test="$n-class = 'S'">
                <xsl:text>//NS</xsl:text>
                <xsl:if test="contains($n-nonuscontrls,'ATOMAL')"><xsl:text>AT</xsl:text></xsl:if>
              </xsl:when>
              <xsl:when test="$n-class = 'C'">
                <xsl:text>//NC</xsl:text>
                <xsl:if test="contains($n-nonuscontrls,'ATOMAL')"><xsl:text>A</xsl:text></xsl:if>
              </xsl:when>
              <xsl:when test="$n-class = 'R'">//NR</xsl:when>
              <xsl:when test="$n-class = 'U'">//NU</xsl:when>
              <xsl:otherwise>
                <xsl:text>UNABLE TO DETERMINE CLASSIFICATION MARKING</xsl:text>
              </xsl:otherwise>
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
                  <xsl:when test="($n-fgiprotect != '') or ($n-fgiopen = 'UNKNOWN')">
                    <xsl:text>FGI</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:value-of select="$n-ownerproducer"/>
                  </xsl:otherwise>
                </xsl:choose>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$n-class"/>
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
      <xsl:variable name="val" select="$n-sci"/>
      <xsl:text>//</xsl:text>
      <xsl:call-template name="get.sci.pm">
        <xsl:with-param name="all" select="$val"/>
        <xsl:with-param name="first" select="substring-before($val,' ')"/>
        <xsl:with-param name="rest" select="substring-after($val,' ')"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the SAR marking **** -->

  <xsl:variable name="sarVal">
    <xsl:if test="$n-sar != ''">
      <xsl:variable name="val" select="$n-sar"/>
      <xsl:text>//</xsl:text>
      <xsl:call-template name="get.sar.pm">
        <xsl:with-param name="all" select="$val"/>
        <xsl:with-param name="first" select="substring-before($val,' ')"/>
        <xsl:with-param name="rest" select="substring-after($val,' ')"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the dissemination marking **** -->

  <xsl:variable name="dissemVal">
    <xsl:if test="$n-dissem != ''">
      <xsl:variable name="val" select="$n-dissem"/>
      <xsl:text>//</xsl:text>
      <xsl:call-template name="get.dissem.pm">
        <xsl:with-param name="all" select="$val"/>
        <xsl:with-param name="first" select="substring-before($val,' ')"/>
        <xsl:with-param name="rest" select="substring-after($val,' ')"/>
        <xsl:with-param name="relto" select="$n-releaseto"/>
        <xsl:with-param name="overalldissem" select="$n-overalldissem"/>
        <xsl:with-param name="overallrelto" select="$n-overallreleaseto"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the non-IC marking **** -->

  <xsl:variable name="nonicVal">
    <xsl:if test="$n-nonic != ''">
      <xsl:variable name="val" select="$n-nonic"/>
      <xsl:text>//</xsl:text>
      <xsl:call-template name="get.nonic.pm">
        <xsl:with-param name="all" select="$val"/>
        <xsl:with-param name="first" select="substring-before($val,' ')"/>
        <xsl:with-param name="rest" select="substring-after($val,' ')"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <!-- **** Output the values as a single string **** -->

  <xsl:value-of select="$classVal"/>
  <xsl:value-of select="$sciVal"/>
  <xsl:value-of select="$sarVal"/>
  <xsl:value-of select="$dissemVal"/>
  <xsl:value-of select="$nonicVal"/>

</xsl:template>

<!-- ********************************************************** -->
<!-- A recursion routine for processing SCIcontrols name tokens -->
<!-- ********************************************************** -->

<xsl:template name="get.sci.pm">
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
        <xsl:when test="(contains($first,'SI-G') or contains($first,'SI-ECI')) and contains($rest,'SI-ECI')">
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
              <xsl:when test="contains($first,'SI-G') or contains($first,'SI-ECI')">
                <xsl:call-template name="get.sci.pm">
                  <xsl:with-param name="first" select="substring-before($rest,' ')"/>
                  <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
                  <xsl:with-param name="flagg" select="$flagg + 1"/>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>
                <xsl:call-template name="get.sci.pm">
                  <xsl:with-param name="first" select="substring-before($rest,' ')"/>
                  <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
                  <xsl:with-param name="flagg" select="$flagg"/>
                </xsl:call-template>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.sci.pm">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="flagg" select="$flagg"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="(starts-with($first,'SI-G') or starts-with($first,'SI-ECI')) and starts-with($rest,'SI-ECI')">
            <xsl:call-template name="get.sci.pm">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="flagg" select="$flagg + 1"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.sci.pm">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="flagg" select="$flagg"/>
            </xsl:call-template>
          </xsl:otherwise> 
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ************************************************************* -->
<!-- A recursion routine for processing EYES dissemination control -->
<!-- ************************************************************* -->

<xsl:template name="get.eyes.pm">
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:value-of select="$first"/>
  <xsl:if test="$rest">
    <xsl:value-of select="'/'"/>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="get.eyes.pm">
          <xsl:with-param name="first" select="substring-before($rest,' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get.eyes.pm">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ************************************************************ -->
<!-- A recursion routine for processing REL dissemination control -->
<!-- ************************************************************ -->

<xsl:template name="get.relto.pm">
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$rest">
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:value-of select="$first"/>
          <xsl:value-of select="', '"/>
          <xsl:call-template name="get.relto.pm">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$first"/>
          <xsl:call-template name="get.relto.pm">
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

<!-- ******************************************************************* -->
<!-- A recursion routine for processing disseminationControl name tokens -->
<!-- ******************************************************************* -->

<xsl:template name="get.dissem.pm">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>
  <xsl:param name="relto"/>
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallrelto"/>
  <xsl:param name="flagg" select="1"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:if test="not(($first = 'RD' and (contains($rest,'RD-CNWDI') or contains($rest,'RD-SG'))) or ($first = 'FRD' and (contains($rest,'FRD-CNWDI') or contains($rest,'FRD-SG'))))">
        <xsl:choose>
          <xsl:when test="(starts-with($first,'RD-SG') or starts-with($first,'FRD-SG')) and $flagg &gt; 1">
            <xsl:text>SG </xsl:text>
            <xsl:value-of select="substring-after($first,'SG-')"/>
          </xsl:when>
          <xsl:when test="starts-with($first,'RD-SG') and $flagg = 1">
            <xsl:text>RD-SG </xsl:text>
            <xsl:value-of select="substring-after($first,'SG-')"/>
          </xsl:when>
          <xsl:when test="starts-with($first,'FRD-SG') and $flagg = 1">
            <xsl:text>FRD-SG </xsl:text>
            <xsl:value-of select="substring-after($first,'SG-')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.dissem.names">
              <xsl:with-param name="name" select="$first"/>
              <xsl:with-param name="rel" select="$relto"/>
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
          <xsl:text>RD-SG </xsl:text>
          <xsl:value-of select="substring-after($all,'SG-')"/>
        </xsl:when>
        <xsl:when test="starts-with($all,'FRD-SG')">
          <xsl:text>FRD-SG </xsl:text>
          <xsl:value-of select="substring-after($all,'SG-')"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="get.dissem.names">
            <xsl:with-param name="name" select="$all"/>
            <xsl:with-param name="rel" select="$relto"/>
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
          <xsl:when test="((starts-with($first,'RD-CNWDI') or starts-with($first,'RD-SG')) and starts-with($rest,'RD-SG')) or ((starts-with($first,'FRD-CNWDI') or starts-with($first,'FRD-SG')) and starts-with($rest,'FRD-SG'))">
            <xsl:call-template name="get.dissem.pm">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
              <xsl:with-param name="flagg" select="$flagg + 1"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.dissem.pm">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="((starts-with($first,'RD-CNWDI') or starts-with($first,'RD-SG')) and starts-with($rest,'RD-SG')) or ((starts-with($first,'FRD-CNWDI') or starts-with($first,'FRD-SG')) and starts-with($rest,'FRD-SG'))">
            <xsl:call-template name="get.dissem.pm">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
              <xsl:with-param name="flagg" select="$flagg + 1"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="get.dissem.pm">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>
          </xsl:otherwise> 
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- *************************************************** -->
<!-- Determine releasableTo name tokens for REL and EYES -->
<!-- *************************************************** -->

<xsl:template name="get.dissem.names">
  <xsl:param name="name"/>
  <xsl:param name="rel"/>
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallrelto"/>

  <xsl:choose>
    <xsl:when test="$name='REL'">
      <xsl:choose>
        <xsl:when test="($rel != '') and (contains($rel,' '))">
          <xsl:variable name="relVal">
            <xsl:call-template name="get.relto.pm">
              <xsl:with-param name="first" select="substring-before($rel,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rel,' ')"/>
            </xsl:call-template>
          </xsl:variable>
          <xsl:choose>
            <xsl:when test="(contains($overalldissem,'REL') and ($overallrelto = $rel))">
              <xsl:text>REL</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>REL TO </xsl:text>
              <xsl:value-of select="$relVal"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>UNABLE TO DETERMINE RELEASABILITY</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$name='EYES'">
      <xsl:choose>
        <xsl:when test="($rel != '') and (contains($rel,' '))">
          <xsl:variable name="eyesVal">
            <xsl:call-template name="get.eyes.pm">
              <xsl:with-param name="first" select="substring-before($rel,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rel,' ')"/>
            </xsl:call-template>
          </xsl:variable>
          <xsl:choose>
            <xsl:when test="(contains($overalldissem,'EYES') and ($overallrelto = $rel))">
              <xsl:text>EYES</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$eyesVal"/>
              <xsl:text> EYES ONLY</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>UNABLE TO DETERMINE EYES ONLY MARKINGS</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$name"/>
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>

<!-- ********************************************************** -->
<!-- A recursion routine for processing nonIC name token values -->
<!-- ********************************************************** -->

<xsl:template name="get.nonic.pm">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:value-of select="$first"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$all"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="$rest">
    <xsl:text>/</xsl:text>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="get.nonic.pm">
          <xsl:with-param name="first" select="substring-before($rest,' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get.nonic.pm">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ****************************************************************** -->
<!-- A recursion routine for processing SARIdentifier name token values -->
<!-- ****************************************************************** -->

<xsl:template name="get.sar.pm">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:text>SAR-</xsl:text>
      <xsl:value-of select="$first"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>SAR-</xsl:text>
      <xsl:value-of select="$all"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="$rest">
    <xsl:text>/</xsl:text>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="get.sar.pm">
          <xsl:with-param name="first" select="substring-before($rest,' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="get.sar.pm">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- ********************************************************** -->
<!-- A generic template for getting a complete portion marking  -->
<!--                                                            -->
<!-- This template can be called without any parameters from    -->
<!-- any stylesheet when the element for which a portion        -->
<!-- marking is required is the current node.  When this        -->
<!-- template is called, the output will include parentheses    -->
<!-- and a space after the portion marking.                     -->
<!-- ********************************************************** -->

<!-- **************************************************************** -->
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
<!-- **************************************************************** -->

<xsl:template name="get.portionmark">
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallreleaseto"/>

  <xsl:text>(</xsl:text>
  <xsl:call-template name="portionmark">
    <xsl:with-param name="class" select="./@classification"/>
    <xsl:with-param name="ownerproducer" select="./@ownerProducer"/>
    <xsl:with-param name="sci" select="./@SCIcontrols"/>
    <xsl:with-param name="sar" select="./@SARIdentifier"/>
    <xsl:with-param name="fgiopen" select="./@FGIsourceOpen"/>
    <xsl:with-param name="fgiprotect" select="./@FGIsourceProtected"/>
    <xsl:with-param name="dissem" select="./@disseminationControls"/>
    <xsl:with-param name="releaseto" select="./@releasableTo"/>
    <xsl:with-param name="nonic" select="./@nonICmarkings"/>
    <xsl:with-param name="overalldissem" select="$overalldissem"/>
    <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
  </xsl:call-template>
  <xsl:text>) </xsl:text>

</xsl:template>

<xsl:template name="get.portionmark.wxs">
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallreleaseto"/>

  <xsl:text>(</xsl:text>
  <xsl:call-template name="portionmark">
    <xsl:with-param name="class" select="./@ism:classification"/>
    <xsl:with-param name="ownerproducer" select="./@ism:ownerProducer"/>
    <xsl:with-param name="sci" select="./@ism:SCIcontrols"/>
    <xsl:with-param name="sar" select="./@ism:SARIdentifier"/>
    <xsl:with-param name="fgiopen" select="./@ism:FGIsourceOpen"/>
    <xsl:with-param name="fgiprotect" select="./@ism:FGIsourceProtected"/>
    <xsl:with-param name="dissem" select="./@ism:disseminationControls"/>
    <xsl:with-param name="releaseto" select="./@ism:releasableTo"/>
    <xsl:with-param name="nonic" select="./@ism:nonICmarkings"/>
    <xsl:with-param name="nonuscontrols" select="./@ism:nonUSControls"/>
    <xsl:with-param name="overalldissem" select="$overalldissem"/>
    <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
  </xsl:call-template>
  <xsl:text>) </xsl:text>

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
<!-- Version 2.0.2                                                    -->
<!--                                                                  -->
<!--   - Modified to correct an FGI marking bug                       -->
<!--                                                                  -->
<!-- Version 2.0.3                                                    -->
<!--                                                                  -->
<!--   - Modified to account for the correct marking format for       -->
<!--     multiple SAR markings.  The "SAR-" prefix must be used with  -->
<!--     each SAR program trigraph or digraph in portion markings.    -->
<!--                                                                  -->
<!--   - Modified to account for the correct marking format for       -->
<!--     multiple non-IC markings.  The delimiter is "/" (not ",").   -->
<!--                                                                  -->
<!-- Version 2.0.3.1                                                  -->
<!--                                                                  -->
<!--   - Modified "get.sci.pm" template to account for the correct    -->
<!--     marking format when both "SI-G" and "SI-ECI-XXX" are         -->
<!--     indicated.                                                   -->
<!--                                                                  -->
<!--   - Modified "get.sci.pm" template to account for the correct    -->
<!--     marking format when "SI" is indicated with either "SI-G" or  -->
<!--     "SI-ECI-XXX" or both.                                        -->
<!--                                                                  -->
<!--   - Modified "get.dissem.pm" template to account for the correct -->
<!--     marking format when both "RD-CNWDI" and "RD-SG-X[X]" are     -->
<!--     indicated, and when both "FRD-CNWDI" and "FRD-SG-X[X]" are   -->
<!--     indicated.                                                   -->
<!--                                                                  -->
<!--   - Modified "get.dissem.pm" template to account for the correct -->
<!--     marking format when "RD" is incorrectly indicated with       -->
<!--     either "RD-CNWDI" or "RD-SG-X[X]" or both, and when "FRD"    -->
<!--     is incorrectly indicated with either "FRD-CNWDI" or          -->
<!--     "FRD-SG-X[X]" or both.                                       -->
<!--                                                                  -->
<!--   - Added a template (get.portionmark.wxs), for use with WXS     -->
<!--     implementations, to return a portion marking for the current -->
<!--     node without the need for parameters corresponding to the    -->
<!--     node's IC ISM security attributes.  Also added "ism"         -->
<!--     namespace declaration.                                       -->
<!--                                                                  -->
<!-- Version 2.1                                                      -->
<!--                                                                  -->
<!--   - Corresponds to IC ISM 2.1 (ISM-XML 1.0).                     -->
<!--                                                                  -->
<!--   - Modified "get.sci.pm" template to more appropriately account -->
<!--     for non-published SCI control markings.                      -->
<!--                                                                  -->
<!--   - Modified stylesheet to render an FGI portion marking when    -->
<!--     @ownerProducer is "NATO" and @classification is not a NATO   -->
<!--     classification but is still a non-US classification          -->
<!--     (i.e. - "TS", "S", "C", "R" or "U").  For example, when      -->
<!--     @ownerProducer is "NATO" and @classification is "S" (not     -->
<!--     "NS" which is the "NATO SECRET" NATO classification), the    -->
<!--     portion marking would be (//NATO S).  According to CAPCO,    -->
<!--     this is a valid FGI portion marking, just as when the value  -->
<!--     of @ownerProducer is any valid registered international      -->
<!--     organization tetragraph.                                     -->
<!--                                                                  -->
<!-- 2010-04-09                                                       -->
<!--   - Modified stylesheet to render (//FGI S) portion mark when 
         ism:FGIsourceProtected='FGI'  and 
         ism:ownerProducer='USA' and
         ism:classification='S' 
         instead of the current rendering of (S)
  -->
<!-- 2010-05-31                                                       -->
<!--   - Modified stylesheet to NATO markings according to ISM.XML.V4 
         new attribute nonUSControls.
-->

<!-- **************************************************************** -->

<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->
