<?xml version="1.0" encoding="GB2312"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!--
	<xsl:import href="function.xsl"/>
-->
	<xsl:variable name="SplitToken">;;</xsl:variable>
	<xsl:template match="/">
		<html>
			<head>
				<title>
					<xsl:value-of select="/Article/@Name"/>
				</title>
			</head>
			<link type="text/css" href="duilib.css" rel="stylesheet"/>
			<body>
				<xsl:call-template name="ListIndex"/>
				<xsl:call-template name="PartIndex"/>
			</body>
		</html>
	</xsl:template>
	<xsl:template name="ListIndex">
		<table style="width:100%">
			<a class="title">
				<xsl:attribute name="name">#0</xsl:attribute>
							Duilib控件属性列表
						</a>
			<xsl:for-each select="/Controls/*">
				<tr>
					<th width="40%">
						<xsl:value-of select="position()"/>
						<a>
							<xsl:attribute name="name">#__<xsl:value-of select="name()"/></xsl:attribute>
						</a>
					</th>
					<th>
						<a >
							<xsl:attribute name="href">#<xsl:value-of select="name()"/></xsl:attribute>
							<xsl:value-of select="name()"/>
						</a>
					</th>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>
	<xsl:template name="PartIndex">
		<table>
			<xsl:for-each select="/Controls/*">
				<tr>
					<td width="200px" class="div_title">控件名</td>
					<a>
						<xsl:attribute name="name">#<xsl:value-of select="name()"/></xsl:attribute>
					</a>
					<td width="200px" class="div_title">父控件名</td>
					<a>
						<xsl:attribute name="name">#<xsl:value-of select="name()"/></xsl:attribute>
					</a>
					<td class="div_title">消息事件</td>
					<a>
						<xsl:attribute name="name">#<xsl:value-of select="name()"/></xsl:attribute>
					</a>
					<td width="200px">
						<a>
							<xsl:attribute name="href">#<xsl:value-of select="name()"/></xsl:attribute>
							返回列表
						</a>
						<xsl:call-template name="PartTitle">
							<xsl:with-param name="name" select="@Index"/>
							<xsl:with-param name="pos" select="position()"/>
						</xsl:call-template>
					</td>
				</tr>
				<tr>
					<td class="td_data">
						<xsl:value-of select="name()" />
					</td>
					<td class="td_data" colspan="1" >
						<xsl:value-of select="./@parent"/>
					</td>
					<td class="td_data" colspan="1" >
						<xsl:value-of select="./@notifies"/>
					</td>
				</tr>
				<tr>
					<td colspan="4">
						<table width="100%">
							<tr>
								<td width="200px" class="table_titleA">属性名</td>
								<td width="100px" class="table_titleA">默认值</td>
								<td width="60px" class="table_titleA">类型</td>
								<td class="table_titleA">注释</td>
							</tr>
							<xsl:for-each select="./Attribute">
								<tr>
									<td>
										<xsl:value-of select="./@name"/>
									</td>
									<td>
										<xsl:value-of select="./@default"/>
									</td>
									<td>
										<xsl:value-of select="./@type"/>
									</td>
									<td>
										<xsl:value-of select="./@comment"/>
									</td>
								</tr>
							</xsl:for-each>
						</table>
					</td>
				</tr>
				<tr>
					<td colspan="4" height="20px" class="table_title"/>
				</tr>
			</xsl:for-each>
			<br/>
		</table>
	</xsl:template>
	<xsl:template name="IndexGroup">
		<xsl:param name="th1" select="''"/>
		<xsl:param name="th2" select="''"/>
		<xsl:param name="th3" select="''"/>
		<tr>
			<th>
				<xsl:call-template name="AddLink">
					<xsl:with-param name="str" select="$th1"/>
				</xsl:call-template>
				<xsl:if test="./preceding::*[position()=1]/@Volume">
					<font class="Volume">
						<xsl:call-template name="AddLink"/>&#160;&#160;[<xsl:value-of select="./preceding::*[position()=1]/@Volume"/>]
					</font>
				</xsl:if>
			</th>
			<th>
				<xsl:call-template name="AddLink">
					<xsl:with-param name="str" select="$th2"/>
				</xsl:call-template>
				<xsl:if test="./@Volume">
					<font class="Volume">
						<xsl:call-template name="AddLink"/>&#160;&#160;[<xsl:value-of select="./@Volume"/>]
					</font>
				</xsl:if>
			</th>
			<th>
				<xsl:call-template name="AddLink">
					<xsl:with-param name="str" select="$th3"/>
				</xsl:call-template>
				<xsl:if test="./following::*[position()=1]/@Volume">
					<font class="Volume">
						<xsl:call-template name="AddLink"/>&#160;&#160;[<xsl:value-of select="./following::*[position()=1]/@Volume"/>]
					</font>
				</xsl:if>
			</th>
		</tr>
	</xsl:template>
	<xsl:template name="AddLink">
		<xsl:param name="str" select="''"/>
		<a href="#{substring-before($str,$SplitToken)}">
			<xsl:value-of select="substring-after($str,$SplitToken)"/>
		</a>
	</xsl:template>
	<xsl:template name="PartTitle">
		<xsl:param name="name" select="''"/>
		<xsl:param name="pos">0</xsl:param>
		<a class="PartIndex" name="{$pos}">
			<xsl:value-of select="$name"/>
		</a>
		<xsl:if test="(count(/Controls/*)>1)">
			<a href="#{-1+$pos}">前移</a>
			<a href="#{1+$pos}">后移</a>
			<a href="#0">返回</a>
		</xsl:if>
	</xsl:template>
	<xsl:template name="ChangeLine">
		<xsl:param name="str"/>
		<xsl:param name="pat"/>
&#12288;&#12288;
		<xsl:choose>
			<xsl:when test="contains($str, $pat)">
				<xsl:if test="not(starts-with($str, $pat))">
					<xsl:value-of select="substring-before($str, $pat)"/>
					<br/>
				</xsl:if>
				<xsl:call-template name="ChangeLine">
					<xsl:with-param name="str" select="substring-after($str, $pat)"/>
					<xsl:with-param name="pat" select="$pat"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$str"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
