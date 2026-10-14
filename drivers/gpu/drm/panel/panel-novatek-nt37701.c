// SPDX-License-Identifier: GPL-2.0-only
/*
 * Panel driver for the Novatek NT37701 AMOLED DSI panel
 *
 * (C) Copyright 2026 fwlta <monkeyfwlta@protonmail.com>
 */

#include <drm/display/drm_dsc.h>
#include <drm/display/drm_dsc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

struct novatek_nt37701 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct drm_dsc_config dsc;
	struct gpio_desc *reset_gpio;
	struct regulator_bulk_data *supplies;
};

static const struct regulator_bulk_data novatek_nt37701_supplies[] = {
	{ .supply = "vddio" },
	{ .supply = "vci" },
	{ .supply = "vdd" },
};

static inline struct novatek_nt37701 *to_novatek_nt37701(struct drm_panel *panel)
{
	return container_of(panel, struct novatek_nt37701, panel);
}

static void novatek_nt37701_reset(struct novatek_nt37701 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(11000, 21000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(11000, 21000);
}

static void novatek_nt37701_on(struct mipi_dsi_multi_context *dsi_ctx)
{
	dsi_ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	/* page unlock: f0 55 aa 52 08 00 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x06);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb5, 0x2c, 0x12, 0x35);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x10);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb5, 0x12, 0x12, 0x12, 0x12, 0x12);
	/* page unlock: f0 55 aa 52 08 00 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb2, 0x11);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x0f);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb2, 0x60, 0x50, 0x66, 0x91, 0x86, 0x91);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb3, 0x00, 0x08, 0x01, 0x5f, 0x01, 0x5f, 0x02, 0xa4, 0x02, 0xa4, 0x03, 0xbb);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x0c);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb3, 0x03, 0xbb, 0x05, 0x2f, 0x05, 0x2f, 0x06, 0x91, 0x06, 0x91, 0x06, 0x92);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x18);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xb3, 0x06, 0x92, 0x0a, 0x2f, 0x0a, 0x2f, 0x0d, 0xb9, 0x0d, 0xb9, 0x0f, 0xff);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x58, 0x00);
	/* page unlock: f0 55 aa 52 08 00 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x08);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xc3, 0x04);
	/* page unlock: f0 55 aa 52 08 03 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x03);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xbc, 0x11, 0x00, 0x09, 0x51);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x04);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xbc, 0x00, 0x09, 0x51, 0x00, 0x09, 0x51, 0x00, 0x0b, 0x41);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x0d);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xbc, 0x00, 0x11, 0x51);
	/* page unlock: f0 55 aa 52 08 00 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xc6, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66);
	/* page unlock: f0 55 aa 52 08 00 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x1c);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xc0, 0x15, 0x0a, 0x38, 0x27, 0x49);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x03, 0x01);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x90, 0x01);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x91, 0xab, 0x28, 0x00, 0x0c, 0xc2, 0x00, 0x03, 0x1c, 0x01, 0x7e, 0x00, 0x0f, 0x08, 0xbb, 0x04, 0x3d, 0x10, 0xf0);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x2c, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x01);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x88, 0x02, 0x1c, 0x08, 0x78);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x5f, 0x00);
	/* skip brightness/cabc: 51 00 00 */
	/* skip brightness/cabc: 53 20 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0x35, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x2a, 0x00, 0x00, 0x04, 0x37);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x2b, 0x00, 0x00, 0x09, 0x5f);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x2f, 0x08);
	/* page unlock: f0 55 aa 52 08 08 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x08);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe1, 0x00, 0x03, 0x03, 0x03, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe3, 0xc0, 0x0e, 0xf3, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x04);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xc1, 0x00);
	/* page unlock: f0 55 aa 52 08 00 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xc0, 0x0e, 0xb3, 0x21);
	/* page unlock: f0 55 aa 52 08 08 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x08);
	mipi_dsi_dcs_write_seq_multi(ctx, 0x6f, 0x03);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xc1, 0x02);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xce, 0x22, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	/* skip sleep-out (framework sends it) */
	mipi_dsi_msleep(ctx, 120);
	/* page unlock: f0 55 aa 52 08 08 */
	mipi_dsi_dcs_write_seq_multi(ctx, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x08);
	mipi_dsi_dcs_write_seq_multi(ctx, 0xe1, 0x63);
	/* skip display-on (framework sends it) */

	mipi_dsi_dcs_exit_sleep_mode_multi(dsi_ctx);
	mipi_dsi_msleep(dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(dsi_ctx);
	mipi_dsi_msleep(dsi_ctx, 20);
}


static int novatek_nt37701_off(struct novatek_nt37701 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);

	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int novatek_nt37701_prepare(struct drm_panel *panel)
{
	struct novatek_nt37701 *ctx = to_novatek_nt37701(panel);
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };
	struct drm_dsc_picture_parameter_set pps;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(novatek_nt37701_supplies),
				    ctx->supplies);
	if (ret < 0)
		return ret;

	novatek_nt37701_reset(ctx);
	novatek_nt37701_on(&dsi_ctx);
	drm_dsc_pps_payload_pack(&pps, &ctx->dsc);
	mipi_dsi_picture_parameter_set_multi(&dsi_ctx, &pps);
	mipi_dsi_compression_mode_multi(&dsi_ctx, true);
	if (dsi_ctx.accum_err) {
		ret = dsi_ctx.accum_err;
		goto err;
	}

	msleep(28);

	return 0;

err:
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(novatek_nt37701_supplies),
			       ctx->supplies);

	return ret;
}

static int novatek_nt37701_unprepare(struct drm_panel *panel)
{
	struct novatek_nt37701 *ctx = to_novatek_nt37701(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = novatek_nt37701_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	regulator_bulk_disable(ARRAY_SIZE(novatek_nt37701_supplies),
			       ctx->supplies);

	return 0;
}

static const struct drm_display_mode novatek_nt37701_mode = {
	.clock = (1080 + 32 + 16 + 32) * (2400 + 4 + 12 + 4) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 32,
	.hsync_end = 1080 + 32 + 16,
	.htotal = 1080 + 32 + 16 + 32,
	.vdisplay = 2400,
	.vsync_start = 2400 + 4,
	.vsync_end = 2400 + 4 + 12,
	.vtotal = 2400 + 4 + 12 + 4,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int novatek_nt37701_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector,
						    &novatek_nt37701_mode);
}

static const struct drm_panel_funcs novatek_nt37701_panel_funcs = {
	.prepare = novatek_nt37701_prepare,
	.unprepare = novatek_nt37701_unprepare,
	.get_modes = novatek_nt37701_get_modes,
};

static int novatek_nt37701_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness_large(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops novatek_nt37701_bl_ops = {
	.update_status = novatek_nt37701_bl_update_status,
};

static struct backlight_device *
novatek_nt37701_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 4095,
		.max_brightness = 4095,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &novatek_nt37701_bl_ops, &props);
}

static int novatek_nt37701_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct novatek_nt37701 *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct novatek_nt37701, panel,
				   &novatek_nt37701_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(novatek_nt37701_supplies),
					    novatek_nt37701_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB101010;
	dsi->mode_flags = MIPI_DSI_MODE_NO_EOT_PACKET | MIPI_DSI_CLOCK_NON_CONTINUOUS;

	ctx->panel.prepare_prev_first = true;
	ctx->panel.backlight = novatek_nt37701_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	/* This panel only supports DSC; unconditionally enable it */
	dsi->dsc = &ctx->dsc;
	ctx->dsc.dsc_version_major = 1;
	ctx->dsc.dsc_version_minor = 1;
	ctx->dsc.slice_height = 12;
	ctx->dsc.slice_width = 1080;
	ctx->dsc.slice_count = 1080 / ctx->dsc.slice_width;
	ctx->dsc.bits_per_component = 10;
	ctx->dsc.bits_per_pixel = 8 << 4; /* 4 fractional bits */
	ctx->dsc.block_pred_enable = true;

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void novatek_nt37701_remove(struct mipi_dsi_device *dsi)
{
	struct novatek_nt37701 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id novatek_nt37701_of_match[] = {
	{ .compatible = "novatek,nt37701" },
	{}
};
MODULE_DEVICE_TABLE(of, novatek_nt37701_of_match);

static struct mipi_dsi_driver novatek_nt37701_driver = {
	.probe = novatek_nt37701_probe,
	.remove = novatek_nt37701_remove,
	.driver = {
		.name = "panel-novatek-nt37701",
		.of_match_table = novatek_nt37701_of_match,
	},
};
module_mipi_dsi_driver(novatek_nt37701_driver);

MODULE_AUTHOR("fwlta <monkeyfwlta@protonmail.com>");
MODULE_DESCRIPTION("Panel driver for the Novatek NT37701/NT37810 AMOLED DSI panel");
MODULE_LICENSE("GPL");
