<?xml version="1.0" encoding="utf-8"?>
<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->
<!-- ****************************************************************
  INTELLIGENCE COMMUNITY TECHNICAL SPECIFICATION  
  XML DATA ENCODING SPECIFICATION FOR 
  INFORMATION SECURITY MARKING METADATA (ISM.XML)
  ******************************************************************* -->
<!-- Module:     IC-ISM-ClassDeclass.xsl                              -->
<!-- Date:     2011-08-12                                             -->
<!-- Creators: Office of the Director of National Intelligence
     Intelligence Community Chief Information Officer                 -->
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
<!-- Order 13526.                                                     -->
<!-- **************************************************************** -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                xmlns:ism="urn:us:gov:ic:ism">

  <!--***********************************************-->
  <!-- Generate the Classification Authority Block for the current element-->
  <!--***********************************************-->
  <xsl:template match="*[@ism:*]" mode="ism:authority">
    <xsl:param name="delimiter" />
    <xsl:call-template name="get.class.declass" >
      <xsl:with-param name="delimiter" select="$delimiter"/>
    </xsl:call-template>
  </xsl:template>

  <!-- **************************************************************** -->
  <!-- get.class.declass - Calls template class.declass with parameters from the element's ISM attributes-->
  <!-- **************************************************************** -->
  <xsl:template name="get.class.declass">
    <xsl:param name="delimiter" />
        
    <xsl:call-template name="class.declass">
      <xsl:with-param name="classification" select="@ism:classification" />
      <xsl:with-param name="classifiedby" select="@ism:classifiedBy"/>
      <xsl:with-param name="derivativelyclassifiedby" select="@ism:derivativelyClassifiedBy"/>
      <xsl:with-param name="classificationreason" select="@ism:classificationReason"/>
      <xsl:with-param name="derivedfrom" select="@ism:derivedFrom"/>
      <xsl:with-param name="declassdate" select="@ism:declassDate"/>
      <xsl:with-param name="declassexception" select="@ism:declassException"/>
      <xsl:with-param name="declassevent" select="@ism:declassEvent"/>
      <xsl:with-param name="typeofexemptedsource" select="@ism:typeOfExceptedSource"/>
      <xsl:with-param name="dateofexemptedsource" select="@ism:dateOfExceptedSource"/>
    <xsl:with-param name="delimiter" select="$delimiter" />
    </xsl:call-template>
    
  </xsl:template>

<!-- **************************************************************** -->
<!-- class.declass - Determines the class/declass block content and   -->
<!--                 calls a template to concatenate the content into -->
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

  <xsl:if test="($n-classification and ($n-classification != 'U') and ($n-classification != 'NU')) or
                ($n-classifiedby or $n-classificationreason or $n-derivedfrom or
                $n-declassdate or $n-declassexception or $n-declassevent or 
                $n-typeofexemptedsource or $n-dateofexemptedsource)">
    <xsl:variable name="warning-missing-classif">
      <xsl:if test="$n-classification = ''">
        <xsl:text>(&#xA0;WARNING! This document does not contain a required overall classification marking.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning-unclass-and-classified-markings">
      <xsl:if test="($n-classification = 'U') ">
        <xsl:text>(&#xA0;WARNING! This document contains overall markings for both an unclassified and a classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning-missing-classifiedBy">
      <xsl:if test="$n-classification and ($n-classification != 'U') and ($n-classifiedby = '') and ($n-derivedfrom = '')">
        <xsl:text>(&#xA0;WARNING! This document does not contain required markings for either an originally or derivatively classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="classified-by-line" select="$n-classifiedby"/>
    <xsl:variable name="derivatively-classified-by-line" select="$n-derivativelyclassifiedby"/>
    <xsl:variable name="derived-from-line" select="$n-derivedfrom"/>
    <xsl:variable name="reason-line" select="$n-classificationreason"/>
    <xsl:variable name="warning-both-original-and-derivatively-classified">
      <xsl:if test="($n-classification != 'U') and $n-classifiedby and $n-derivedfrom">
        <xsl:text>(&#xA0;WARNING! This document contains markings for both an originally and derivatively classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning-missing-classificationReason">
      <xsl:if test="$n-classification and ($n-classification != 'U')  and $n-classifiedby and ($n-classificationreason = '')">
        <xsl:text>(&#xA0;WARNING! The reason for the classification decision should be specified for an originally classified document.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning-missing-declass-instructions">
      <xsl:if test="$n-classification and ($n-classification != 'U') and
                    ($n-declassdate = '') and ($n-declassexception = '') and ($n-declassevent = '') and ($n-typeofexemptedsource = '')">
        <xsl:text>(&#xA0;WARNING! This document does not contain required declassification instructions or markings.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning-missing-declass-info">
      <xsl:if test="$n-classification and ($n-classification != 'U')  
        and $n-declassexception  
        and not(contains($n-declassexception,'AEA')) 
        and not(contains($n-declassexception,'NATO')) 
        and not(contains($n-declassexception,'NATO-AEA')) 
        and not(contains($n-declassexception,'25X1-human'))
        and not(contains($n-declassexception,'50X1-HUM')) 
        and not(contains($n-declassexception,'50X2-WMD'))
        and ($n-declassdate = '') 
        and ($n-declassevent = '')">
        <xsl:text>(&#xA0;WARNING! A declassification date or event should be specified for a document with a 25X or 50X declassification exemption, unless the document has a declassification exemption of 25X1-human, 50X1-HUM, 50X2-WMD, AEA, or  NATO.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="declassify-on-line">
      <xsl:if test="$n-declassexception">
        <xsl:choose>
          <xsl:when test="$n-declassexception = 'AEA'">
            <xsl:text>Not Applicable to RD/FRD/TFNI portions. See source list for NSI portions.</xsl:text>
          </xsl:when>
          <xsl:when test="$n-declassexception = 'NATO'">
            <xsl:text>Not Applicable to NATO portions. See source list for NSI portions.</xsl:text>
          </xsl:when>
          <xsl:when test="$n-declassexception = 'NATO-AEA'">
            <xsl:text>Not Applicable to RD/FRD/TFNI and NATO portions. See source list for NSI portions.</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$n-declassexception"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:if test="$n-declassdate">
        <xsl:if test="$n-declassexception">
          <xsl:text>, </xsl:text>
        </xsl:if>
        <xsl:call-template name="ism:format.date">
          <xsl:with-param name="date" select="substring($n-declassdate,1,10)"/>
        </xsl:call-template>
      </xsl:if>
      <xsl:if test="$n-declassevent">
        <xsl:if test="$n-declassexception and ($n-declassdate = '')">
          <xsl:text>, </xsl:text>
        </xsl:if>
        <xsl:if test="$n-declassdate">
          <xsl:text> or </xsl:text>
        </xsl:if>
        <xsl:value-of select="$n-declassevent"/>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="($n-typeofexemptedsource = '') and $n-dateofexemptedsource">
          <xsl:if test="$n-declassdate or $n-declassexception or $n-declassevent">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:text>Date of Source: </xsl:text>
          <xsl:call-template name="ism:format.date">
            <xsl:with-param name="date" select="substring($n-dateofexemptedsource,1,10)"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:when test="$n-typeofexemptedsource and ($n-dateofexemptedsource = '')">
          <xsl:if test="$n-declassdate or $n-declassexception or $n-declassevent">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:text>Source marked </xsl:text>
          <xsl:choose>
            <xsl:when test="contains($n-typeofexemptedsource,' ')">
              <xsl:call-template name="ism:NMTOKENS-to-CSV">
                <xsl:with-param name="txt" select="$n-typeofexemptedsource"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$n-typeofexemptedsource"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:when test="$n-typeofexemptedsource and $n-dateofexemptedsource">
          <xsl:if test="$n-declassdate or $n-declassexception or $n-declassevent">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:text>Source marked </xsl:text>
          <xsl:choose>
            <xsl:when test="contains($n-typeofexemptedsource,' ')">
              <xsl:call-template name="ism:NMTOKENS-to-CSV">
                <xsl:with-param name="txt" select="$n-typeofexemptedsource"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$n-typeofexemptedsource"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>, Date of Source: </xsl:text>
          <xsl:call-template name="ism:format.date">
            <xsl:with-param name="date" select="substring($n-dateofexemptedsource,1,10)"/>
          </xsl:call-template>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="warning-missing-exempted-source-info">
      <xsl:choose>
        <xsl:when test="($n-typeofexemptedsource = '') and $n-dateofexemptedsource">
          <xsl:text>(&#xA0;WARNING! The exempted source marking should be included when the date of exempted source is specified.&#xA0;)</xsl:text>
        </xsl:when>
        <xsl:when test="$n-typeofexemptedsource and ($n-dateofexemptedsource = '')">
          <xsl:text>(&#xA0;WARNING! The date of exempted source marking should be included when the exempted source marking is specified.&#xA0;)</xsl:text>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="warning-invalid-declass-date-and-exemption">
      <xsl:if test="$n-declassdate and (contains($n-declassexception,'25X1-human'))">
        <xsl:text>(&#xA0;WARNING! This document contains both a declassification date and a declassification exemption of 25X1-human.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="warning-invalid-declass-event-and-exemption">
      <xsl:if test="$n-declassevent and (contains($n-declassexception,'25X1-human'))">
        <xsl:text>(&#xA0;WARNING! This document contains both a declassification event and a declassification exemption of 25X1-human.&#xA0;)</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:call-template name="ism:concat.class.declass">
      <xsl:with-param name="warning-missing-classif" select="string($warning-missing-classif)"/>
      <xsl:with-param name="warning-unclass-and-classified-markings" select="string($warning-unclass-and-classified-markings)"/>
      <xsl:with-param name="warning-missing-classifiedBy" select="string($warning-missing-classifiedBy)"/>
      <xsl:with-param name="warning-both-original-and-derivatively-classified" select="string($warning-both-original-and-derivatively-classified)"/>
      <xsl:with-param name="warning-missing-classificationReason" select="string($warning-missing-classificationReason)"/>
      <xsl:with-param name="warning-missing-declass-instructions" select="string($warning-missing-declass-instructions)"/>
      <xsl:with-param name="warning-missing-declass-info" select="string($warning-missing-declass-info)"/>
      <xsl:with-param name="warning-missing-exempted-source-info" select="string($warning-missing-exempted-source-info)"/>
      <xsl:with-param name="warning-invalid-declass-date-and-exemption" select="string($warning-invalid-declass-date-and-exemption)"/>
      <xsl:with-param name="warning-invalid-declass-event-and-exemption" select="string($warning-invalid-declass-event-and-exemption)"/>
      <xsl:with-param name="classified-by-line" select="string($classified-by-line)"/>
      <xsl:with-param name="derivatively-classified-by-line" select="string($derivatively-classified-by-line)"/>
      <xsl:with-param name="derived-from-line" select="string($derived-from-line)"/>
      <xsl:with-param name="reason-line" select="string($reason-line)"/>
      <xsl:with-param name="declassify-on-line" select="string($declassify-on-line)"/>
      <xsl:with-param name="delimiter" select="$class-declass-delimiter"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="ism:concat.class.declass">
  <!-- **************************************************************** -->
  <!-- concat.class.declass - Concatenates class/declass block content  -->
  <!--                        into a delimited string                   -->
  <!-- **************************************************************** -->
  <xsl:param name="warning-missing-classif"/>
  <xsl:param name="warning-unclass-and-classified-markings"/>
  <xsl:param name="warning-missing-classifiedBy"/>
  <xsl:param name="warning-both-original-and-derivatively-classified"/>
  <xsl:param name="warning-missing-classificationReason"/>
  <xsl:param name="warning-missing-declass-instructions"/>
  <xsl:param name="warning-missing-declass-info"/>
  <xsl:param name="warning-missing-exempted-source-info"/>
  <xsl:param name="warning-invalid-declass-date-and-exemption"/>
  <xsl:param name="warning-invalid-declass-event-and-exemption"/>
  <xsl:param name="classified-by-line"/>
  <xsl:param name="derivatively-classified-by-line"/>
  <xsl:param name="derived-from-line"/>
  <xsl:param name="reason-line"/>
  <xsl:param name="declassify-on-line"/>
  <xsl:param name="delimiter"/>

  <xsl:variable name="class-declass-content">
    <xsl:if test="$warning-missing-classif">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-missing-classif"/>
    </xsl:if>
    <xsl:if test="$warning-unclass-and-classified-markings">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-unclass-and-classified-markings"/>
    </xsl:if>
    <xsl:if test="$warning-missing-classifiedBy">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-missing-classifiedBy"/>
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
    <xsl:if test="$classified-by-line or $derived-from-line">
      <xsl:value-of select="$delimiter"/>
      <xsl:choose>
        <xsl:when test="$classified-by-line">
          <xsl:text>Classified by: </xsl:text>
          <xsl:value-of select="$classified-by-line"/>
        </xsl:when>
        <xsl:when test="$derived-from-line">
          <xsl:if test="$derivatively-classified-by-line">
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
    <xsl:if test="$reason-line">
      <xsl:value-of select="$delimiter"/>
      <xsl:text>Reason: </xsl:text>
      <xsl:value-of select="$reason-line"/>
    </xsl:if>

    <!-- **************************************************************** -->
    <xsl:if test="$warning-both-original-and-derivatively-classified">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-both-original-and-derivatively-classified"/>
    </xsl:if>
    <xsl:if test="$warning-missing-classificationReason">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-missing-classificationReason"/>
    </xsl:if>
    <xsl:if test="$warning-missing-declass-instructions">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-missing-declass-instructions"/>
    </xsl:if>

    <!-- **************************************************************** -->
    <!-- Include the "Declassify on" line.                                -->
    <!-- **************************************************************** -->
    <xsl:if test="$declassify-on-line">
      <xsl:value-of select="$delimiter"/>
      <xsl:text>Declassify on: </xsl:text>
      <xsl:value-of select="$declassify-on-line"/>
    </xsl:if>

    <!-- **************************************************************** -->
    <xsl:if test="$warning-missing-declass-info">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-missing-declass-info"/>
    </xsl:if>
    <xsl:if test="$warning-missing-exempted-source-info">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-missing-exempted-source-info"/>
    </xsl:if>
    <xsl:if test="$warning-invalid-declass-date-and-exemption">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-invalid-declass-date-and-exemption"/>
    </xsl:if>
    <xsl:if test="$warning-invalid-declass-event-and-exemption">
      <xsl:value-of select="$delimiter"/>
      <xsl:value-of select="$warning-invalid-declass-event-and-exemption"/>
    </xsl:if>
  </xsl:variable>

  <xsl:value-of select="substring-after($class-declass-content,$delimiter)"/>

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
  
  <!-- ************************************************************ -->
  <!-- Generate a display-friendly rendition of an ISO-8601 date -->
  <!-- ************************************************************ -->
<xsl:template name="ism:format.date">
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
<!--     concatenated into a delimited text string, where each        -->
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
<!-- 2010-09-24                                                   
  - Changed the name of warning variables to be more descriptive.
  - Namespace qualified templates, except for class.declass and get.class.declass.  
-->
<!-- 2011-01-28                                                   
  - Added convenience template with mode="ism:authority" for processing current element to generate Classification Authority Block
  - Changed namespace for qualified templates to use ISM namespace, except for class.declass and get.class.declass (preserved for compatibility).  
-->
<!-- 2011-08-12
  - Changed logic for declass exception warnings to include 25X1-HUM and 25X2-WMD
-->
<!-- 2013-01-02
    Removed NU since it is no longer a classification.
    Added logic for NATO and NATO-AEA as a new exemptions.

-->
<!-- **************************************************************** -->

<!-- **************************************************************** -->
<!--                            UNCLASSIFIED                          -->
<!-- **************************************************************** -->