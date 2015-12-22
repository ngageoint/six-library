<?xml version="1.0" encoding="utf-8"?>

<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->

<!-- **************************************************************** -->
<!--             IC Information Security Marking Standard             -->
<!--         Classificatiion/Declassification Block Stylesheet        -->
<!--                                                                  -->
<!-- File:     IC-ISM-ClassDeclass-v2.1.xsl                           -->
<!-- Schema:   Corresponds to IC ISM v2.1                             -->
<!-- Version:  2.1                                                    -->
<!-- Date:     2008-06-03                                             -->
<!-- Creators: Science Applications International Corporation         -->
<!--           (for the ICIS DSCA CMSTT)                              -->
<!-- **************************************************************** -->

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
<!-- This stylesheet outputs classification/declassification block    -->
<!-- content including the "Classified by", "Reason", "Derived from", -->
<!-- and/or "Declassify on" information required by the CAPCO         -->
<!-- Implementation Manual pursuant to ISOO Directive 1 and Executive -->
<!-- Order 12958.                                                     -->
<!-- **************************************************************** -->

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
<!--   - Modified to separate the rendering of content as HTML into   -->
<!--     an independent template                                      -->
<!--                                                                  -->
<!--   - Other minor modifications                                    -->
<!--                                                                  -->
<!-- Version 2.0.2                                                    -->
<!--                                                                  -->
<!--   - Modified so that content is not rendered into any specific   -->
<!--     format by this stylesheet.  Instead the content is simply    -->
<!--     concatinated into a delimited text string, where each        -->
<!--     section of the delimited string is a line of content within  -->
<!--     the class/declass block, including various warning messages. -->
<!--     This method leaves it up to the calling stylesheet to parse  -->
<!--     the delimited string and render it according to the desired  -->
<!--     output.  A delimiter can be set in the calling stylesheet    -->
<!--     and passed into the class.declass template as a parameter by -->
<!--     the template call in the calling stylesheet.  If a delimiter -->
<!--     is not set and/or not passed in, the delimiter is set to a   -->
<!--     "|" in the class.declass template in this stylesheet, and    -->
<!--     the calling stylesheet MUST parse the string based on this   -->
<!--     delimiter.                                                   -->
<!--                                                                  -->
<!-- Version 2.1                                                      -->
<!--                                                                  -->
<!--   - Corresponds to IC ISM 2.1 (ISM-XML 1.0).                     -->
<!--                                                                  -->
<!--   - Modified the "Declassify on" line (declassify-on-line) to    -->
<!--     exclude the optional time zone indicator which may exist in  -->
<!--     the @declassDate and @dateOfExemptedSource attribute values. -->
<!--                                                                  -->
<!--   - Added a template to convert date values from "YYYY-MM-DD"    -->
<!--     format to "Month [D]D, YYYY" format for the @declassDate and -->
<!--     @dateOfExemptedSource attributes.                            -->
<!--                                                                  -->
<!--   - Added a recursive template to include a space after each     -->
<!--     comma when multiple declassification exemption markings or   -->
<!--     multiple type of exempted source markings are specified.     -->
<!--                                                                  -->
<!--   - Added a warning when a 25X declassification exemption (other -->
<!--     than 25X1-human) is specified and a declassification date or -->
<!--     declassification event is not specified.                     -->
<!--                                                                  -->
<!--   - Modified stylesheet to account for @derivativelyClassifiedBy -->
<!--     attribute.                                                   -->
<!--                                                                  -->
<!-- **************************************************************** -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

<!-- **************************************************************** -->
<!-- class.declass - Determines the class/declass block content and   -->
<!--                 calls a template to concatinate the content into -->
<!--                 a delimited string                               -->
<!-- **************************************************************** -->

<xsl:template name="class.declass">
  <xsl:param name="classification"/>
  <xsl:param name="classifiedby"/>
  <xsl:param name="derivativelyclassifiedby"/>
  <xsl:param name="classificationreason"/>
  <xsl:param name="derivedfrom"/>
  <xsl:param name="declassdate"/>
  <xsl:param name="declassexception"/>
  <xsl:param name="declassevent"/>
  <xsl:param name="typeofexemptedsource"/>
  <xsl:param name="dateofexemptedsource"/>
  <xsl:param name="delimiter"/>

  <xsl:variable name="class-declass-delimiter">
    <xsl:choose>
      <xsl:when test="not($delimiter) or ($delimiter = '')">
        <xsl:text>|</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$delimiter"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="n-classification" select="normalize-space($classification)"/>
  <xsl:variable name="n-classifiedby" select="normalize-space($classifiedby)"/>
  <xsl:variable name="n-derivativelyclassifiedby" select="normalize-space($derivativelyclassifiedby)"/>
  <xsl:variable name="n-classificationreason" select="normalize-space($classificationreason)"/>
  <xsl:variable name="n-derivedfrom" select="normalize-space($derivedfrom)"/>
  <xsl:variable name="n-declassdate" select="normalize-space($declassdate)"/>
  <xsl:variable name="n-declassexception" select="normalize-space($declassexception)"/>
  <xsl:variable name="n-declassevent" select="normalize-space($declassevent)"/>
  <xsl:variable name="n-typeofexemptedsource" select="normalize-space($typeofexemptedsource)"/>
  <xsl:variable name="n-dateofexemptedsource" select="normalize-space($dateofexemptedsource)"/>

  <xsl:if test="(($n-classification != '') and ($n-classification != 'U') and ($n-classification != 'NU')) or
                (($n-classifiedby != '') or ($n-classificationreason != '') or ($n-derivedfrom != '') or
                ($n-declassdate != '') or ($n-declassexception != '') or ($n-declassevent != '') or 
                ($n-typeofexemptedsource != '') or ($n-dateofexemptedsource != ''))">
    <xsl:variable name="warning01">
      <xsl:if test="$n-classification = ''">
        <xsl:text>(&#xA0;WARNING! This document does not contain a required overall classification marking.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning02">
      <xsl:if test="($n-classification = 'U') or ($n-classification = 'NU')">
        <xsl:text>(&#xA0;WARNING! This document contains overall markings for both an unclassified and a classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning03">
      <xsl:if test="($n-classification != '') and ($n-classification != 'U') and ($n-classification != 'NU') and ($n-classifiedby = '') and ($n-derivedfrom = '')">
        <xsl:text>(&#xA0;WARNING! This document does not contain required markings for either an originally or derivatively classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="classified-by-line" select="$n-classifiedby"/>
    <xsl:variable name="derivatively-classified-by-line" select="$n-derivativelyclassifiedby"/>
    <xsl:variable name="derived-from-line" select="$n-derivedfrom"/>
    <xsl:variable name="reason-line" select="$n-classificationreason"/>
    <xsl:variable name="warning04">
      <xsl:if test="($n-classification != 'U') and ($n-classification != 'NU') and ($n-classifiedby != '') and ($n-derivedfrom != '')">
        <xsl:text>(&#xA0;WARNING! This document contains markings for both an originally and derivatively classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning05">
      <xsl:if test="($n-classification != '') and ($n-classification != 'U') and ($n-classification != 'NU') and ($n-classifiedby != '') and ($n-classificationreason = '')">
        <xsl:text>(&#xA0;WARNING! The reason for the classification decision should be specified for an originally classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning06">
      <xsl:if test="($n-classification != '') and ($n-classification != 'U') and ($n-classification != 'NU') and
                    ($n-declassdate = '') and ($n-declassexception = '') and ($n-declassevent = '') and ($n-typeofexemptedsource = '')">
        <xsl:text>(&#xA0;WARNING! This document does not contain required declassification instructions or markings.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning07">
      <xsl:if test="($n-classification != '') and ($n-classification != 'U') and ($n-classification != 'NU') and
                    ($n-declassexception != '') and not(contains($n-declassexception,'25X1-human')) and ($n-declassdate = '') and ($n-declassevent = '')">
        <xsl:text>(&#xA0;WARNING! A declassification date or event should be specified for a document with a 25X declassification exemption, unless the document has a declassification exemption of 25X1-human.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="declassify-on-line">
      <xsl:if test="$n-declassexception != ''">
        <xsl:choose>
          <xsl:when test="contains($n-declassexception,' ')">
            <xsl:call-template name="format.multiple.values">
              <xsl:with-param name="first" select="substring-before($n-declassexception,' ')"/>
              <xsl:with-param name="rest" select="substring-after($n-declassexception,' ')"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$n-declassexception"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="$n-declassdate != ''">
        <xsl:if test="$n-declassexception != ''">
          <xsl:text>, </xsl:text>
        </xsl:if>
        <!-- <xsl:value-of select="translate(substring($n-declassdate,1,10),'-','')"/> -->
        <xsl:call-template name="format.date">
          <xsl:with-param name="date" select="substring($n-declassdate,1,10)"/>
        </xsl:call-template>
      </xsl:if>
      <xsl:if test="$n-declassevent != ''">
        <xsl:if test="($n-declassexception != '') and ($n-declassdate = '')">
          <xsl:text>, </xsl:text>
        </xsl:if>
        <xsl:if test="$n-declassdate != ''">
          <xsl:text> or </xsl:text>
        </xsl:if>
        <xsl:value-of select="$n-declassevent"/>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="($n-typeofexemptedsource = '') and
                        ($n-dateofexemptedsource  != '')">
          <xsl:if test="($n-declassdate != '') or
                        ($n-declassexception != '') or
                        ($n-declassevent != '')">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:text>Date of Source: </xsl:text>
          <!-- <xsl:value-of select="translate(substring($n-dateofexemptedsource,1,10),'-','')"/> -->
          <xsl:call-template name="format.date">
            <xsl:with-param name="date" select="substring($n-dateofexemptedsource,1,10)"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:when test="($n-typeofexemptedsource != '') and
                        ($n-dateofexemptedsource = '')">
          <xsl:if test="($n-declassdate != '') or
                        ($n-declassexception != '') or
                        ($n-declassevent != '')">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:text>Source marked </xsl:text>
          <xsl:choose>
            <xsl:when test="contains($n-typeofexemptedsource,' ')">
              <xsl:call-template name="format.multiple.values">
                <xsl:with-param name="first" select="substring-before($n-typeofexemptedsource,' ')"/>
                <xsl:with-param name="rest" select="substring-after($n-typeofexemptedsource,' ')"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$n-typeofexemptedsource"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:when test="($n-typeofexemptedsource != '') and
                        ($n-dateofexemptedsource != '')">
          <xsl:if test="($n-declassdate != '') or
                        ($n-declassexception != '') or
                        ($n-declassevent != '')">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:text>Source marked </xsl:text>
          <xsl:choose>
            <xsl:when test="contains($n-typeofexemptedsource,' ')">
              <xsl:call-template name="format.multiple.values">
                <xsl:with-param name="first" select="substring-before($n-typeofexemptedsource,' ')"/>
                <xsl:with-param name="rest" select="substring-after($n-typeofexemptedsource,' ')"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$n-typeofexemptedsource"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>, Date of Source: </xsl:text>
          <!-- <xsl:value-of select="translate(substring($n-dateofexemptedsource,1,10),'-','')"/> -->
          <xsl:call-template name="format.date">
            <xsl:with-param name="date" select="substring($n-dateofexemptedsource,1,10)"/>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="warning08">
      <xsl:choose>
        <xsl:when test="($n-typeofexemptedsource = '') and
                        ($n-dateofexemptedsource  != '')">
          <xsl:text>(&#xA0;WARNING! The exempted source marking should be included when the date of exempted source is specified.&#xA0;)</xsl:text>
        </xsl:when>
        <xsl:when test="($n-typeofexemptedsource != '') and
                        ($n-dateofexemptedsource = '')">
          <xsl:text>(&#xA0;WARNING! The date of exempted source marking should be included when the exempted source marking is specified.&#xA0;)</xsl:text>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="warning09">
      <xsl:if test="($n-declassdate != '') and (contains($n-declassexception,'25X1-human'))">
        <xsl:text>(&#xA0;WARNING! This document contains both a declassification date and a declassification exemption of 25X1-human.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning10">
      <xsl:if test="($n-declassevent != '') and (contains($n-declassexception,'25X1-human'))">
        <xsl:text>(&#xA0;WARNING! This document contains both a declassification event and a declassification exemption of 25X1-human.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:call-template name="concat.class.declass">
      <xsl:with-param name="warning01" select="string($warning01)"/>
      <xsl:with-param name="warning02" select="string($warning02)"/>
      <xsl:with-param name="warning03" select="string($warning03)"/>
      <xsl:with-param name="warning04" select="string($warning04)"/>
      <xsl:with-param name="warning05" select="string($warning05)"/>
      <xsl:with-param name="warning06" select="string($warning06)"/>
      <xsl:with-param name="warning07" select="string($warning07)"/>
      <xsl:with-param name="warning08" select="string($warning08)"/>
      <xsl:with-param name="warning09" select="string($warning09)"/>
      <xsl:with-param name="warning10" select="string($warning10)"/>
      <xsl:with-param name="classified-by-line" select="string($classified-by-line)"/>
      <xsl:with-param name="derivatively-classified-by-line" select="string($derivatively-classified-by-line)"/>
      <xsl:with-param name="derived-from-line" select="string($derived-from-line)"/>
      <xsl:with-param name="reason-line" select="string($reason-line)"/>
      <xsl:with-param name="declassify-on-line" select="string($declassify-on-line)"/>
      <xsl:with-param name="delimiter" select="$class-declass-delimiter"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="concat.class.declass">
  <!-- **************************************************************** -->
  <!-- concat.class.declass - Concatinates class/declass block content  -->
  <!--                        into a delimited string                   -->
  <!-- **************************************************************** -->
  <xsl:param name="warning01"/>
  <xsl:param name="warning02"/>
  <xsl:param name="warning03"/>
  <xsl:param name="warning04"/>
  <xsl:param name="warning05"/>
  <xsl:param name="warning06"/>
  <xsl:param name="warning07"/>
  <xsl:param name="warning08"/>
  <xsl:param name="warning09"/>
  <xsl:param name="warning10"/>
  <xsl:param name="classified-by-line"/>
  <xsl:param name="derivatively-classified-by-line"/>
  <xsl:param name="derived-from-line"/>
  <xsl:param name="reason-line"/>
  <xsl:param name="declassify-on-line"/>
  <xsl:param name="delimiter"/>

  <xsl:variable name="class-declass-content">
    <xsl:if test="$warning01 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning01"/>
    </xsl:if>
    <xsl:if test="$warning02 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning02"/>
    </xsl:if>
    <xsl:if test="$warning03 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning03"/>
    </xsl:if>

    <!-- **************************************************************** -->
    <!-- Include the "Classified by" line or the "Derived from" line.     -->
    <!--                                                                  -->
    <!-- NOTE: A classified document can be either an originally          -->
    <!--       classified document or a derivatively classified document. -->
    <!--       An originally classified document will always contain a    -->
    <!--       "Classified by" line.  A derivatively classified document  -->
    <!--       may (somewhat misleadingly) contain a "Classified by" line -->
    <!--       if attribute @derivativelyClassifiedBy exists, and will    -->
    <!--       always contain a "Derived from" line.                      -->
    <!-- **************************************************************** -->

    <xsl:if test="($classified-by-line != '') or ($derived-from-line != '')">
      <xsl:value-of select="$delimiter"/>
      <xsl:choose>
        <xsl:when test="$classified-by-line != ''">
          <xsl:text>Classified by: </xsl:text>
          <xsl:value-of select="$classified-by-line"/>
        </xsl:when>
        <xsl:when test="$derived-from-line != ''">
          <xsl:if test="$derivatively-classified-by-line != ''">
            <xsl:text>Classified by: </xsl:text>
            <xsl:value-of select="$derivatively-classified-by-line"/>
            <xsl:value-of select="$delimiter"/>
          </xsl:if>
          <xsl:text>Derived from: </xsl:text>
          <xsl:value-of select="$derived-from-line"/>
        </xsl:when>
      </xsl:choose>
    </xsl:if>

    <!-- **************************************************************** -->

    <!-- **************************************************************** -->
    <!-- Include the "Reason" line.                                       -->
    <!--                                                                  -->
    <!-- NOTE: For originally classified documents, the reason for the    -->
    <!--       classification decision should always be specified.        -->
    <!--                                                                  -->
    <!--       According to ISOO Directive 1, Section 2001.22(c), for     -->
    <!--       derivatively classified documents, the reason for the      -->
    <!--       original classification decision, as reflected in the      -->
    <!--       source document(s) or classification guide, is not         -->
    <!--       required.  If included, however, it shall conform to the   -->
    <!--       standards in Section 2001.21(a)(3).                        -->
    <!--                                                                  -->
    <!--       In other words, the "Reason" line can be included in the   -->
    <!--       classification/declassification block for derivatively     -->
    <!--       classified documents.                                      -->
    <!-- **************************************************************** -->

    <xsl:if test="$reason-line != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:text>Reason: </xsl:text>
      <xsl:value-of select="$reason-line"/>
    </xsl:if>

    <!-- **************************************************************** -->

    <xsl:if test="$warning04 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning04"/>
    </xsl:if>
    <xsl:if test="$warning05 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning05"/>
    </xsl:if>
    <xsl:if test="$warning06 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning06"/>
    </xsl:if>

    <!-- **************************************************************** -->
    <!-- Include the "Declassify on" line.                                -->
    <!-- **************************************************************** -->

    <xsl:if test="$declassify-on-line != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:text>Declassify on: </xsl:text>
      <xsl:value-of select="$declassify-on-line"/>
    </xsl:if>

    <!-- **************************************************************** -->

    <xsl:if test="$warning07 != ''">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning07"/>
    </xsl:if>
    <xsl:if test="$warning08">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning08"/>
    </xsl:if>
    <xsl:if test="$warning09">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning09"/>
    </xsl:if>
    <xsl:if test="$warning10">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning10"/>
    </xsl:if>
  </xsl:variable>

  <xsl:value-of select="substring-after($class-declass-content,$delimiter)"/>

</xsl:template>

<xsl:template name="format.multiple.values">
  <xsl:param name="first"/>
  <xsl:param name="rest"/>

  <xsl:choose>
    <xsl:when test="$rest">
      <xsl:choose>
        <xsl:when test="contains($rest,' ')">
          <xsl:value-of select="$first"/>
          <xsl:value-of select="', '"/>
          <xsl:call-template name="format.multiple.values">
            <xsl:with-param name="first" select="substring-before($rest,' ')"/>
            <xsl:with-param name="rest" select="substring-after($rest,' ')"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$first"/>
          <xsl:call-template name="format.multiple.values">
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

<xsl:template name="format.date">
  <xsl:param name="date"/>

  <xsl:choose>
    <xsl:when test="substring($date,6,2) = '01'">
      <xsl:value-of select=" 'January' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '02'">
      <xsl:value-of select=" 'February' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '03'">
      <xsl:value-of select=" 'March' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '04'">
      <xsl:value-of select=" 'April' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '05'">
      <xsl:value-of select=" 'May' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '06'">
      <xsl:value-of select=" 'June' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '07'">
      <xsl:value-of select=" 'July' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '08'">
      <xsl:value-of select=" 'August' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '09'">
      <xsl:value-of select=" 'September' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '10'">
      <xsl:value-of select=" 'October' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '11'">
      <xsl:value-of select=" 'November' "/>
    </xsl:when>
    <xsl:when test="substring($date,6,2) = '12'">
      <xsl:value-of select=" 'December' "/>
    </xsl:when>
  </xsl:choose>
  <xsl:text> </xsl:text>
  <xsl:choose>
    <xsl:when test="starts-with(substring($date,9,2),'0')">
      <xsl:value-of select="substring($date,10,1)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="substring($date,9,2)"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>, </xsl:text>
  <xsl:value-of select="substring($date,1,4)"/>
</xsl:template>

</xsl:stylesheet>

<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->
