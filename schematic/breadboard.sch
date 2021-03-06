<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.2">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="chips">
<packages>
<package name="DIL40">
<description>&lt;b&gt;Dual In Line Package&lt;/b&gt;</description>
<wire x1="25.4" y1="6.731" x2="-25.4" y2="6.731" width="0.1524" layer="21"/>
<wire x1="-25.4" y1="-6.731" x2="25.4" y2="-6.731" width="0.1524" layer="21"/>
<wire x1="25.4" y1="6.731" x2="25.4" y2="-6.731" width="0.1524" layer="21"/>
<wire x1="-25.4" y1="6.731" x2="-25.4" y2="0.889" width="0.1524" layer="21"/>
<wire x1="-25.4" y1="-6.731" x2="-25.4" y2="-1.143" width="0.1524" layer="21"/>
<wire x1="-25.4" y1="0.889" x2="-25.4" y2="-1.143" width="0.1524" layer="21" curve="-180"/>
<pad name="1" x="-24.13" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="2" x="-21.59" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="7" x="-8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="8" x="-6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="3" x="-19.05" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="4" x="-16.51" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="6" x="-11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="5" x="-13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="9" x="-3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="10" x="-1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="11" x="1.27" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="12" x="3.81" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="13" x="6.35" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="14" x="8.89" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="15" x="11.43" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="16" x="13.97" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="17" x="16.51" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="18" x="19.05" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="19" x="21.59" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="20" x="24.13" y="-7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="21" x="24.13" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="22" x="21.59" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="23" x="19.05" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="24" x="16.51" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="25" x="13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="26" x="11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="27" x="8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="28" x="6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="29" x="3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="30" x="1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="31" x="-1.27" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="32" x="-3.81" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="33" x="-6.35" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="34" x="-8.89" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="35" x="-11.43" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="36" x="-13.97" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="37" x="-16.51" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="38" x="-19.05" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="39" x="-21.59" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<pad name="40" x="-24.13" y="7.62" drill="0.8128" shape="long" rot="R90"/>
<text x="-25.908" y="-6.604" size="1.778" layer="25" rot="R90">&gt;NAME</text>
<text x="-17.145" y="-1.016" size="1.778" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="ATMEGAXX4">
<text x="-20.32" y="25.4" size="1.778" layer="95">&gt;NAME</text>
<text x="-20.32" y="-30.48" size="1.778" layer="96">&gt;VALUE</text>
<pin name="PB0(XCK/T0)" x="-22.86" y="22.86" visible="pin" length="short"/>
<pin name="PB1(CLKO/T1)" x="-22.86" y="20.32" visible="pin" length="short"/>
<pin name="PB2(INT2/AIN0)" x="-22.86" y="17.78" visible="pin" length="short"/>
<pin name="PB3(OC0A/AIN1)" x="-22.86" y="15.24" visible="pin" length="short"/>
<pin name="PB4(OC0B/SS)" x="-22.86" y="12.7" visible="pin" length="short"/>
<pin name="PB5(ICP3/MOSI)" x="-22.86" y="10.16" visible="pin" length="short"/>
<pin name="PB6(OC3A/MISO)" x="-22.86" y="7.62" visible="pin" length="short"/>
<pin name="PB7(OC3B/SCK)" x="-22.86" y="5.08" visible="pin" length="short"/>
<pin name="RESET" x="-22.86" y="2.54" visible="pin" length="short" direction="in" function="dot"/>
<pin name="VCC" x="-22.86" y="0" visible="pin" length="short" direction="pwr"/>
<pin name="GND" x="-22.86" y="-2.54" visible="pin" length="short" direction="pwr"/>
<pin name="XTAL2" x="-22.86" y="-5.08" visible="pin" length="short" direction="out"/>
<pin name="XTAL1" x="-22.86" y="-7.62" visible="pin" length="short" direction="in" function="clk"/>
<pin name="PD0(RXD0/T3)" x="-22.86" y="-10.16" visible="pin" length="short"/>
<pin name="PD1(TXD0)" x="-22.86" y="-12.7" visible="pin" length="short"/>
<pin name="PD2(RXD1/INT0)" x="-22.86" y="-15.24" visible="pin" length="short"/>
<pin name="PD3(TXD1/INT1)" x="-22.86" y="-17.78" visible="pin" length="short"/>
<pin name="PD4(XCK1/OC1B)" x="-22.86" y="-20.32" visible="pin" length="short"/>
<pin name="PD5(OC1A)" x="-22.86" y="-22.86" visible="pin" length="short"/>
<pin name="PD6(OC2B/ICP)" x="-22.86" y="-25.4" visible="pin" length="short"/>
<pin name="PD7(OC2A)" x="22.86" y="-25.4" visible="pin" length="short" rot="R180"/>
<pin name="PC0(SCL)" x="22.86" y="-22.86" visible="pin" length="short" rot="R180"/>
<pin name="PC1(SDA)" x="22.86" y="-20.32" visible="pin" length="short" rot="R180"/>
<pin name="PC2(TCK)" x="22.86" y="-17.78" visible="pin" length="short" rot="R180"/>
<pin name="PC3(TMS)" x="22.86" y="-15.24" visible="pin" length="short" rot="R180"/>
<pin name="PC4(TDO)" x="22.86" y="-12.7" visible="pin" length="short" rot="R180"/>
<pin name="PC5(TDI)" x="22.86" y="-10.16" visible="pin" length="short" rot="R180"/>
<pin name="PC6(TOSC1)" x="22.86" y="-7.62" visible="pin" length="short" rot="R180"/>
<pin name="PC7(TOSC2)" x="22.86" y="-5.08" visible="pin" length="short" rot="R180"/>
<pin name="AVCC" x="22.86" y="-2.54" visible="pin" length="short" direction="pwr" rot="R180"/>
<pin name="AGND" x="22.86" y="0" visible="pin" length="short" direction="pwr" rot="R180"/>
<pin name="AREF" x="22.86" y="2.54" visible="pin" length="short" direction="in" rot="R180"/>
<pin name="PA7(ADC7)" x="22.86" y="5.08" visible="pin" length="short" rot="R180"/>
<pin name="PA6(ADC6)" x="22.86" y="7.62" visible="pin" length="short" rot="R180"/>
<pin name="PA5(ADC5)" x="22.86" y="10.16" visible="pin" length="short" rot="R180"/>
<pin name="PA4(ADC4)" x="22.86" y="12.7" visible="pin" length="short" rot="R180"/>
<pin name="PA3(ADC3)" x="22.86" y="15.24" visible="pin" length="short" rot="R180"/>
<pin name="PA2(ADC2)" x="22.86" y="17.78" visible="pin" length="short" rot="R180"/>
<pin name="PA1(ADC1)" x="22.86" y="20.32" visible="pin" length="short" rot="R180"/>
<pin name="PA0(ADC0)" x="22.86" y="22.86" visible="pin" length="short" rot="R180"/>
<wire x1="-20.32" y1="25.4" x2="20.32" y2="25.4" width="0.254" layer="94"/>
<wire x1="20.32" y1="25.4" x2="20.32" y2="-27.94" width="0.254" layer="94"/>
<wire x1="20.32" y1="-27.94" x2="-20.32" y2="-27.94" width="0.254" layer="94"/>
<wire x1="-20.32" y1="-27.94" x2="-20.32" y2="25.4" width="0.254" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="ATMEGAXX4" prefix="IC" uservalue="yes">
<description>&lt;b&gt;ATmegaXX4&lt;/b&gt;
&lt;br /&gt;
&lt;br /&gt;ATmega164
&lt;br /&gt;ATmega324
&lt;br /&gt;ATmega644
&lt;br /&gt;ATmega1284</description>
<gates>
<gate name="G$1" symbol="ATMEGAXX4" x="0" y="0"/>
</gates>
<devices>
<device name="" package="DIL40">
<connects>
<connect gate="G$1" pin="AGND" pad="31"/>
<connect gate="G$1" pin="AREF" pad="32"/>
<connect gate="G$1" pin="AVCC" pad="30"/>
<connect gate="G$1" pin="GND" pad="11"/>
<connect gate="G$1" pin="PA0(ADC0)" pad="40"/>
<connect gate="G$1" pin="PA1(ADC1)" pad="39"/>
<connect gate="G$1" pin="PA2(ADC2)" pad="38"/>
<connect gate="G$1" pin="PA3(ADC3)" pad="37"/>
<connect gate="G$1" pin="PA4(ADC4)" pad="36"/>
<connect gate="G$1" pin="PA5(ADC5)" pad="35"/>
<connect gate="G$1" pin="PA6(ADC6)" pad="34"/>
<connect gate="G$1" pin="PA7(ADC7)" pad="33"/>
<connect gate="G$1" pin="PB0(XCK/T0)" pad="1"/>
<connect gate="G$1" pin="PB1(CLKO/T1)" pad="2"/>
<connect gate="G$1" pin="PB2(INT2/AIN0)" pad="3"/>
<connect gate="G$1" pin="PB3(OC0A/AIN1)" pad="4"/>
<connect gate="G$1" pin="PB4(OC0B/SS)" pad="5"/>
<connect gate="G$1" pin="PB5(ICP3/MOSI)" pad="6"/>
<connect gate="G$1" pin="PB6(OC3A/MISO)" pad="7"/>
<connect gate="G$1" pin="PB7(OC3B/SCK)" pad="8"/>
<connect gate="G$1" pin="PC0(SCL)" pad="22"/>
<connect gate="G$1" pin="PC1(SDA)" pad="23"/>
<connect gate="G$1" pin="PC2(TCK)" pad="24"/>
<connect gate="G$1" pin="PC3(TMS)" pad="25"/>
<connect gate="G$1" pin="PC4(TDO)" pad="26"/>
<connect gate="G$1" pin="PC5(TDI)" pad="27"/>
<connect gate="G$1" pin="PC6(TOSC1)" pad="28"/>
<connect gate="G$1" pin="PC7(TOSC2)" pad="29"/>
<connect gate="G$1" pin="PD0(RXD0/T3)" pad="14"/>
<connect gate="G$1" pin="PD1(TXD0)" pad="15"/>
<connect gate="G$1" pin="PD2(RXD1/INT0)" pad="16"/>
<connect gate="G$1" pin="PD3(TXD1/INT1)" pad="17"/>
<connect gate="G$1" pin="PD4(XCK1/OC1B)" pad="18"/>
<connect gate="G$1" pin="PD5(OC1A)" pad="19"/>
<connect gate="G$1" pin="PD6(OC2B/ICP)" pad="20"/>
<connect gate="G$1" pin="PD7(OC2A)" pad="21"/>
<connect gate="G$1" pin="RESET" pad="9"/>
<connect gate="G$1" pin="VCC" pad="10"/>
<connect gate="G$1" pin="XTAL1" pad="13"/>
<connect gate="G$1" pin="XTAL2" pad="12"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="IC_MCU" library="chips" deviceset="ATMEGAXX4" device="" value="ATmega1284A"/>
<part name="IC_DMA" library="chips" deviceset="ATMEGAXX4" device="" value="ATmega324A"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="IC_MCU" gate="G$1" x="86.36" y="96.52"/>
<instance part="IC_DMA" gate="G$1" x="185.42" y="96.52"/>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
