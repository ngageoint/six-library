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
  Date:     2011-07-11
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


<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:ism="urn:us:gov:ic:ism">

  <xsl:output method="text" encoding="UTF-8" media-type="text-plain" indent="no"/>
  <!-- If including this xsl causes "Content is not allowed in prolog" the importing 
  XSL is likely missing an output declaration -->
  
  <xsl:param name="warn-missing-classif" select="'MISSING CLASSIFICATION MARKING'" />
  <xsl:param name="warn-parse-classif" select="'UNABLE TO DETERMINE CLASSIFICATION MARKING'" />
  <xsl:param name="warn-parse-ownerproducer" select="concat($warn-parse-classif, ' - MISSING OWNER/PRODUCER')" />
  <xsl:param name="warn-parse-relto" select="'UNABLE TO DETERMINE RELEASABILITY'" />
  <xsl:param name="warn-parse-displayonly" select="'UNABLE TO DETERMINE DISPLAY ONLY'" />
  <xsl:param name="warn-parse-eyes" select="'UNABLE TO DETERMINE EYES ONLY MARKINGS'" />            

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
  <!-- Mode for generating the CAPCO portion mark-->
  <!--***********************************************-->
  <xsl:template match="*[@ism:*]" mode="ism:portionmark">
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallreleaseto"/>
    <xsl:call-template name="get.portionmark" >
      <xsl:with-param name="overalldissem" select="$overalldissem"/>
      <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
    </xsl:call-template>
  </xsl:template>
 
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
  <xsl:param name="joint"/>
  <xsl:param name="sci"/>
  <xsl:param name="sar"/>
  <xsl:param name="atomicenergymarkings" />
  <xsl:param name="fgiopen"/>
  <xsl:param name="fgiprotect"/>
  <xsl:param name="dissem"/>
  <xsl:param name="releaseto"/>
  <xsl:param name="displayonly"/>
  <xsl:param name="nonic"/>
  <xsl:param name="nonuscontrols"/>
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallreleaseto"/>

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
      <xsl:with-param name="by" select="'OC-USGOV'" />
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="n-releaseto" select="normalize-space($releaseto)"/>
  <xsl:variable name="n-displayonly" select="normalize-space($displayonly)"/>
  <xsl:variable name="n-nonic" select="normalize-space($nonic)"/>
  <xsl:variable name="n-nonuscontrls" select="normalize-space($nonuscontrols)"/>
  <xsl:variable name="n-overalldissem" select="normalize-space($overalldissem)"/>
  <xsl:variable name="n-overallreleaseto" select="normalize-space($overallreleaseto)"/>
  
  <!-- **** Determine the classification marking **** -->
  <xsl:variable name="classVal">
    <xsl:choose>
      <xsl:when test="$n-class != ''">
        <xsl:choose>
          <xsl:when test="$n-ownerproducer = ''"><xsl:value-of select="$warn-parse-ownerproducer"/></xsl:when>
          <xsl:when test="contains($n-ownerproducer,' ') and ($n-fgiprotect = '') and $joint='true'">
            <xsl:choose>
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'R') and ($n-class != 'U')">
                <xsl:value-of select="$warn-parse-classif"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>//JOINT </xsl:text>
                <xsl:value-of select="$n-class"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$n-ownerproducer"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="contains($n-ownerproducer,' ') and ($n-fgiprotect = '') and $joint != 'true'">
            <xsl:choose>
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'R') and ($n-class != 'U')">
                <xsl:value-of select="$warn-parse-classif"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>//</xsl:text>
                <xsl:value-of select="$n-ownerproducer"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$n-class"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="($n-ownerproducer = 'USA') and ($n-fgiopen != 'UNKNOWN') and ($n-fgiprotect = '')">
            <xsl:choose>
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'U')">
                <xsl:value-of select="$warn-parse-classif"/>
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
              </xsl:when>
              <xsl:when test="$n-class = 'S'">
                <xsl:text>//NS</xsl:text>
              </xsl:when>
              <xsl:when test="$n-class = 'C'">
                <xsl:text>//NC</xsl:text>
              </xsl:when>
              <xsl:when test="$n-class = 'R'">
                <xsl:text>//NR</xsl:text>
              </xsl:when>
              <xsl:when test="$n-class = 'U'">
                <xsl:text>//NU</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$warn-parse-classif"/>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:if test="$n-nonuscontrls">
              <xsl:text>//</xsl:text>
              <xsl:value-of select="translate($n-nonuscontrls,' ','/')" />
            </xsl:if>      
          </xsl:when>
          <xsl:when test="starts-with($n-ownerproducer, 'NATO:')">
            <xsl:variable name="natoNacString">
              <xsl:call-template name="ism:get.nato.nac.portion">
                <xsl:with-param name="source" select="$n-ownerproducer"/>
              </xsl:call-template>
            </xsl:variable>
            <xsl:choose>
              <xsl:when test="$n-class='S'">
                <xsl:value-of select="concat('//N',$natoNacString,'S')"/>
              </xsl:when>
              <xsl:when test="$n-class='R'">
                <xsl:value-of select="concat('//N',$natoNacString,'R')"/>
              </xsl:when>
              <xsl:when test="$n-class='C'">
                <xsl:value-of select="concat('//N',$natoNacString,'C')"/>
              </xsl:when>
              <xsl:when test="$n-class='U'">
                <xsl:value-of select="concat('//N',$natoNacString,'U')"/>
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
              <xsl:when test="($n-class != 'TS') and ($n-class != 'S') and ($n-class != 'C') and ($n-class != 'R') and ($n-class != 'U')">
                <xsl:value-of select="$warn-parse-classif"/>
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
      <xsl:otherwise><xsl:value-of select="$warn-missing-classif" /></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!-- **** Determine the SCI marking **** -->
  <xsl:variable name="sciVal">
    <xsl:if test="$n-sci != ''">
      <xsl:text>//</xsl:text>
      <xsl:call-template name="ism:get.sci.pm">
        <xsl:with-param name="all" select="$n-sci"/>
        <xsl:with-param name="first" select="substring-before($n-sci,' ')"/>
        <xsl:with-param name="rest" select="substring-after($n-sci,' ')"/>
      </xsl:call-template>
      
      <xsl:if test="$n-nonuscontrls and contains($n-nonuscontrls, 'BALK')">
        <xsl:text>/BALK</xsl:text>
      </xsl:if>
      
      <xsl:if test="$n-nonuscontrls and contains($n-nonuscontrls, 'BOHEMIA')">
        <xsl:text>/BOHEMIA</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine AtomicEnergyMarking ****-->
  <xsl:variable name="atomicEnergyVal">
    <xsl:if test="$n-atomicenergymarkings != ''">
      <xsl:text>//</xsl:text>
      <xsl:call-template name="ism:get.atomicEnergyMarking.pm">
        <xsl:with-param name="all" select="$n-atomicenergymarkings"/>
        <xsl:with-param name="first" select="substring-before($n-atomicenergymarkings,' ')"/>
        <xsl:with-param name="rest" select="substring-after($n-atomicenergymarkings,' ')"/>
      </xsl:call-template>
      <xsl:if test="$n-nonuscontrls and contains($n-nonuscontrls, 'ATOMAL')">
        <xsl:text>/ATOMAL</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:variable>

  <!-- **** Determine the SAR marking **** -->
  <xsl:variable name="sarVal">
    <xsl:if test="$n-sar != ''">
      <xsl:text>//SAR-</xsl:text>
      <xsl:choose>
        <xsl:when test="contains($n-sar,' ')">
          <xsl:call-template name="ism:get.sar.pm">
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

  <!-- **** Determine the dissemination marking **** -->
  <xsl:variable name="dissemVal">
    <xsl:if test="$n-dissem != ''">
      <xsl:variable name="val" select="$n-dissem"/>
      <xsl:text>//</xsl:text>
      <xsl:call-template name="ism:get.dissem.pm">
        <xsl:with-param name="all" select="$val"/>
        <xsl:with-param name="first" select="substring-before($val,' ')"/>
        <xsl:with-param name="rest" select="substring-after($val,' ')"/>
        <xsl:with-param name="relto" select="$n-releaseto"/>
        <xsl:with-param name="displayonly" select="$n-displayonly"/>
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
      <xsl:call-template name="ism:get.nonic.pm">
        <xsl:with-param name="all" select="$val"/>
        <xsl:with-param name="first" select="substring-before($val,' ')"/>
        <xsl:with-param name="rest" select="substring-after($val,' ')"/>
      </xsl:call-template>
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
      test="(($n-ownerproducer = 'USA') or (contains($n-ownerproducer,'USA') and $n-fgiprotect != ''))">
      <xsl:choose>
        <xsl:when
          test="(($n-fgiopen != '') and (not(contains($n-fgiopen,'UNKNOWN'))) and ($n-fgiprotect = ''))">

          <xsl:text>//FGI </xsl:text>
          <xsl:value-of select="translate($n-fgiopen,'_:','  ')"/>
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

  <!-- **** Output the values as a single string **** -->
  <xsl:value-of select="$classVal"/>
  <xsl:value-of select="$sciVal"/>
  <xsl:value-of select="$sarVal"/>
  <xsl:value-of select="$atomicEnergyVal"/>
  <xsl:value-of select="$fgiVal"/>
  <xsl:value-of select="$dissemVal"/>
  <xsl:value-of select="$nonicVal"/>

</xsl:template>
  
  <!-- ********************************************************** -->
  <!-- A recursion routine for processing SCIcontrols name tokens -->
  <!-- ********************************************************** -->
  <xsl:template name="ism:get.sci.pm">
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
          <xsl:call-template name="ism:get.sci.pm">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.sci.pm">
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise> 
      </xsl:choose>
    </xsl:if>
    
  </xsl:template>

<!-- ******************************************************************* -->
<!-- A recursion routine for processing disseminationControl name tokens -->
<!-- ******************************************************************* -->
<xsl:template name="ism:get.dissem.pm">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>
  <xsl:param name="relto"/>
  <xsl:param name="displayonly" />
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallrelto"/>
  <xsl:param name="flagg" select="1"/>

  <xsl:choose>
    <xsl:when test="$first">

            <xsl:call-template name="ism:get.dissem.names">
              <xsl:with-param name="name" select="$first"/>
              <xsl:with-param name="rel" select="$relto"/>
              <xsl:with-param name="displayonly" select="$displayonly" />
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>

    </xsl:when>
    <xsl:otherwise>
   
          <xsl:call-template name="ism:get.dissem.names">
            <xsl:with-param name="name" select="$all"/>
            <xsl:with-param name="rel" select="$relto"/>
            <xsl:with-param name="displayonly" select="$displayonly" />
            <xsl:with-param name="overalldissem" select="$overalldissem"/>
            <xsl:with-param name="overallrelto" select="$overallrelto"/>
          </xsl:call-template>
        </xsl:otherwise>
    
  </xsl:choose>
  <xsl:if test="$rest">
      <!--Each dissemination control is separated by a slash -->
     <xsl:text>/</xsl:text>

    <xsl:choose>
      <xsl:when test="contains($rest,' ')">

            <xsl:call-template name="ism:get.dissem.pm">
              <xsl:with-param name="first" select="substring-before($rest,' ')"/>
              <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="displayonly" select="$displayonly"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>

      </xsl:when>
      <xsl:otherwise>
      
            <xsl:call-template name="ism:get.dissem.pm">
              <xsl:with-param name="first" select="$rest"/>
              <xsl:with-param name="relto" select="$relto"/>
              <xsl:with-param name="displayonly" select="$displayonly"/>
              <xsl:with-param name="overalldissem" select="$overalldissem"/>
              <xsl:with-param name="overallrelto" select="$overallrelto"/>
            </xsl:call-template>
          </xsl:otherwise> 

      </xsl:choose>
  </xsl:if>

</xsl:template>

<!-- *************************************************** -->
<!-- Determine releasableTo name tokens for REL and EYES -->
<!-- *************************************************** -->
<xsl:template name="ism:get.dissem.names">
  <xsl:param name="name"/>
  <xsl:param name="rel"/>
  <xsl:param name="displayonly" />
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallrelto"/>

  <xsl:choose>
    <xsl:when test="$name='REL'">
      <xsl:choose>
        <xsl:when test="($rel != '') and (contains($rel,' '))">
          <xsl:variable name="relVal">
            <xsl:call-template name="ism:NMTOKENS-to-CSV">
              <xsl:with-param name="text" select="$rel" />
            </xsl:call-template>
          </xsl:variable>
          <xsl:choose>
            <xsl:when test="(contains($overalldissem,'REL') and ($overallrelto = $rel))">
              <xsl:text>REL</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>REL TO </xsl:text>
              <xsl:value-of select="translate($relVal,':_','  ')"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="$warn-parse-relto"/></xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$name='EYES'">
      <xsl:choose>
        <xsl:when test="($rel != '') and (contains($rel,' '))">
          <xsl:variable name="eyesVal">
            <xsl:call-template name="ism:replace">
              <xsl:with-param name="text" select="$rel"/>
              <xsl:with-param name="find" select="' '"/>
              <xsl:with-param name="replace" select="'/'"/>
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
        <xsl:otherwise><xsl:value-of select="$warn-parse-eyes"/></xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$name='DISPLAYONLY'">
      <xsl:text>DISPLAY ONLY </xsl:text>
      <xsl:choose>
        <xsl:when test="($displayonly != '')">
          <xsl:choose>
            <xsl:when test="contains($displayonly, ' ')">
              <xsl:variable name="displayWithComma">
                <xsl:call-template name="ism:NMTOKENS-to-CSV">
                  <xsl:with-param name="text" select="$displayonly" />
                </xsl:call-template>
              </xsl:variable>
              <xsl:value-of select="translate($displayWithComma,':_','  ')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="translate($displayonly,':_','  ')" />
            </xsl:otherwise>
          </xsl:choose>     
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
  
  <!-- ************************************************** -->
  <!-- A recursion routine for processing SAR name tokens -->
  <!-- ************************************************** -->
  <xsl:template name="ism:get.sar.pm">
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
          <xsl:call-template name="ism:get.sar.pm">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.sar.pm">
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    
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

<!-- ********************************************************** -->
<!-- A recursion routine for processing nonIC name token values -->
<!-- ********************************************************** -->
<xsl:template name="ism:get.nonic.pm">
  <xsl:param name="all"/>
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$first">
      <xsl:choose>
      	<xsl:when test="starts-with($first, 'ACCM-')">
      		<!-- Remove ACCM prefix from ACCM tokens -->
      		<xsl:variable name="prefixlessACCM" select="substring-after($first, 'ACCM-')"/>
      		<!-- Replace '_' with ' ' -->
      		<xsl:value-of select="translate($prefixlessACCM, '_', ' ')"/>
      	</xsl:when>
      	<xsl:otherwise>
      		<xsl:value-of select="$first"/>
      	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
    	<xsl:choose>
    		<xsl:when test="starts-with($all, 'ACCM-')">
    			<!-- Remove ACCM prefix from ACCM tokens -->
    			<xsl:variable name="prefixlessACCM" select="substring-after($first, 'ACCM-')"/>
    			<!-- Replace '_' with ' ' -->
    			<xsl:value-of select="translate($prefixlessACCM, '_', ' ')"/>
    		</xsl:when>
    		<xsl:otherwise>
    			<xsl:value-of select="$all"/>
    		</xsl:otherwise>
    	</xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="$rest">
    <xsl:text>/</xsl:text>
    <xsl:choose>
      <xsl:when test="contains($rest,' ')">
        <xsl:call-template name="ism:get.nonic.pm">
          <xsl:with-param name="first" select="substring-before($rest,' ')"/>
          <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="ism:get.nonic.pm">
          <xsl:with-param name="first" select="$rest"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>

</xsl:template>


  <!-- ************************************************** -->
  <!-- A recursion routine for processing atomicEnergyMarking tokens -->
  <!-- ************************************************** -->
  <xsl:template name="ism:get.atomicEnergyMarking.pm">
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
                
                <xsl:variable name="previous" select="substring-before($all, $first)" />
                <xsl:variable name="sigmaTypeToken" select="concat(' ', substring-before($first, concat('-',substring-after($subcompartment,'-'))))" />
                <!--When this is the first SIGMA value for this control -->
                <xsl:if test="not(contains($previous, $sigmaTypeToken))" >
                  <xsl:text>-SG</xsl:text>
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
          <xsl:call-template name="ism:get.atomicEnergyMarking.pm">
            <xsl:with-param name="all" select="$all"/>
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="ism:get.atomicEnergyMarking.pm">
            <xsl:with-param name="all" select="$all"/>
            <xsl:with-param name="first" select="$rest"/>
          </xsl:call-template>
        </xsl:otherwise> 
      </xsl:choose>
    </xsl:if>
    
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
    <xsl:param name="text" />
    <xsl:call-template name="ism:replace">
      <xsl:with-param name="text" select="$text" />
      <xsl:with-param name="find" select="' '" />
      <xsl:with-param name="replace" select="', '" />
    </xsl:call-template>
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
<!--       As per CAPCO guidelines, "REL TO" and "EYES ONLY" portion -->
<!--       markings can be abbreviated when they would otherwise be     -->
<!--       identical to the corresponding document-level markings.            -->
<!--                                                                                                                          -->
<!--       The "overalldissem" and "overallreleaseto" parameters are        -->
<!--       not required.  However, if the parameters are not passed            -->
<!--       into the template, a comparison can not be made, in which        -->
<!--       case the full "REL TO" or "EYES ONLY" dissemination                -->
<!--       control markings will be rendered for the portion even                 -->
<!--       when the portion-level and document-level dissemination        -->
<!--       control markings are the same.                                                       -->
<!-- **************************************************************** -->
<xsl:template name="get.portionmark">
  <xsl:param name="overalldissem"/>
  <xsl:param name="overallreleaseto"/>

  <xsl:text>(</xsl:text>
  <xsl:call-template name="portionmark">
    <xsl:with-param name="class" select="./@ism:classification"/>
    <xsl:with-param name="ownerproducer" select="./@ism:ownerProducer"/>
    <xsl:with-param name="joint" select="./@ism:joint"/>
    <xsl:with-param name="sci" select="./@ism:SCIcontrols"/>
    <xsl:with-param name="sar" select="./@ism:SARIdentifier"/>
    <xsl:with-param name="atomicenergymarkings" select="./@ism:atomicEnergyMarkings"/>
    <xsl:with-param name="fgiopen" select="./@ism:FGIsourceOpen"/>
    <xsl:with-param name="fgiprotect" select="./@ism:FGIsourceProtected"/>
    <xsl:with-param name="dissem" select="./@ism:disseminationControls"/>
    <xsl:with-param name="releaseto" select="./@ism:releasableTo"/>
    <xsl:with-param name="displayonly" select="./@ism:displayOnlyTo"/>
    <xsl:with-param name="nonic" select="./@ism:nonICmarkings"/>
    <xsl:with-param name="nonuscontrols" select="./@ism:nonUSControls"/>
    <xsl:with-param name="overalldissem" select="$overalldissem"/>
    <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
  </xsl:call-template>
  <xsl:text>) </xsl:text>

</xsl:template>
  
  <xsl:template name="get.portionmark.wxs">
    <xsl:param name="overalldissem"/>
    <xsl:param name="overallreleaseto"/>
    
    <xsl:call-template name="get.portionmark">
      <xsl:with-param name="overalldissem" select="$overalldissem"/>
      <xsl:with-param name="overallreleaseto" select="$overallreleaseto"/>
    </xsl:call-template>
    
  </xsl:template>
  
  <!-- ************************************************************ -->
  <!-- Get the NATO NAC string                                      -->
  <!-- ************************************************************ -->
  <xsl:template name="ism:get.nato.nac.portion">
    <xsl:param name="source"/>
    <xsl:value-of select="document('nacs.xml')//nacs/nac[@name=substring-after($source, ':')]/@portion"/>
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
<!-- 2010-08-27                                                       -->
<!--   - Modified stylesheet to use XSL parameters for warning messages, 
            providing the opportunity to customize warning values.
-->
<!-- 2010-09-24                                                   
   - Incorporate Atomic Energy Controls, displayonly in ISM.XML.v5 
     and changes to NATO portion marks.
   - Namespace qualified templates, except for portionmark, get.portionmark, and get.portionmark.wxs.  
-->
<!-- 2011-01-28                                                   
  - Added support for @ism:ACCM
  - Corrected rendering of @ism:SARIdentifier
  - Added convenience template with mode="ism:portionmark"
  - Changed namespace qualifier for templates to use ISM namespace, except for portionmark, get.portionmark, and get.portionmark.wxs (for legacy support).  
-->
<!-- 2011-07-11                                                   
  - Removed @ism:ACCM. ACCM values are to be expresed in @ism:nonICmarkings.
-->
<!-- 2013-02-15                                                   
  - Added logic to take the joint attribute into consideration when rendering.
-->
<!-- 2014-06-19                                                  
  - Corrected "OC OC-USGOV" rendering for task #196.
-->
<!-- 2015-11-23
  - Corrected BALK, BOHEMIA, ATOMAL rendering for task #1100 
-->
<!-- **************************************************************** -->

<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->