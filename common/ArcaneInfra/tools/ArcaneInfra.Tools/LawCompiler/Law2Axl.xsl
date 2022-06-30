<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="xml" indent="yes"/>

  <xsl:template match="law">
    <service name="{@name}" version="1.0" type="caseoption">
      <interface name="Law::IFunction" />
      <options>
      <complex name="properties" type="Properties" minOccurs="0" maxOccurs="1">
        <xsl:apply-templates select="inputs" />
        <xsl:apply-templates select="outputs" />
        <xsl:apply-templates select="parameters" />
      </complex>
      <xsl:apply-templates select="options" />
      </options>
    </service>
  </xsl:template>

  <xsl:template match="*">
    <xsl:copy-of select="."/>
  </xsl:template>

  <xsl:template match="options">
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="inputs">
    <xsl:if test="count(*) > 0">
      <complex name="inputs" type="Inputs" minOccurs="0" maxOccurs="1">
      <xsl:apply-templates select="property" />
      </complex>
    </xsl:if> 
  </xsl:template>
    
  <xsl:template match="outputs">
    <xsl:if test="count(*) > 0">
      <complex name="outputs" type="Outputs" minOccurs="0" maxOccurs="1">
      <xsl:apply-templates select="property" />
      </complex>
    </xsl:if>
  </xsl:template>
    
  <xsl:template match="parameters">
    <xsl:if test="count(*) > 0">
      <complex name="parameters" type="Parameters" minOccurs="0" maxOccurs="1">
      <xsl:apply-templates select="property" />
      </complex>
    </xsl:if>
  </xsl:template>

  <xsl:template match="property">
    <complex name="{@name}" type="Complex{translate(@name,'-','_')}" minOccurs="0" maxOccurs="1">
      <simple name="name" type="string" optional="true" />
      <simple name="id" type="integer" optional="true" />
      <simple name="path" type="string" optional="true" />
    </complex>
  </xsl:template>

</xsl:stylesheet>