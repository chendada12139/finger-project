module Sign_G(

	//  ADC  //
	output		ADC_CONVST,
	output		ADC_SCK,
	output		ADC_SDI,
	input 		ADC_SDO,
	// CLOCK //
	input 		CLK_50,
	// UART0 //
	input			rxd,
	output		txd,
	// UART1 //
	input			rxd_1,
	output		txd_1,
	//  KEY  //
	input 		KEY

);

DE10_NANO_qsys u0 (
        .adc_ltc2308_CONVST     (ADC_CONVST),     //     adc_ltc2308.CONVST
        .adc_ltc2308_SCK        (ADC_SCK),        //                .SCK
        .adc_ltc2308_SDI        (ADC_SDI),        //                .SDI
        .adc_ltc2308_SDO        (ADC_SDO),        //                .SDO
        .clk_clk                (CLK_50),         //             clk.clk
        .reset_reset_n          (KEY),            //           reset.reset_n
        .uart_0_rxd             (rxd),            //          uart_0.rxd
        .uart_0_txd             (txd),            //                .txd
        .uart_1_rxd             (rxd_1),          //          uart_1.rxd
        .uart_1_txd             (txd_1)           //                .txd
    ); 
	 
endmodule