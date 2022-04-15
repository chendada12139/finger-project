
module DE10_NANO_qsys (
	adc_ltc2308_CONVST,
	adc_ltc2308_SCK,
	adc_ltc2308_SDI,
	adc_ltc2308_SDO,
	clk_clk,
	pll_qsys_locked_export,
	reset_reset_n,
	uart_0_rxd,
	uart_0_txd,
	uart_1_rxd,
	uart_1_txd);	

	output		adc_ltc2308_CONVST;
	output		adc_ltc2308_SCK;
	output		adc_ltc2308_SDI;
	input		adc_ltc2308_SDO;
	input		clk_clk;
	output		pll_qsys_locked_export;
	input		reset_reset_n;
	input		uart_0_rxd;
	output		uart_0_txd;
	input		uart_1_rxd;
	output		uart_1_txd;
endmodule
