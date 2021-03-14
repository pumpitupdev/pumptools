#
# Distribution build rules
#

builddir        := $(BUILDDIR)
zipdir          := $(BUILDDIR)/zip

$(zipdir)/:
	$(V)mkdir -p $@

################################################
# Hooks: Lexical sorting

$(zipdir)/exchook.zip: \
		$(builddir)/bin/exchook.so \
		dist/conf/exchook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/f2hook.zip: \
		$(builddir)/bin/f2hook.so \
		dist/conf/f2hook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/fexhook.zip: \
		$(builddir)/bin/fexhook.so \
		dist/conf/fexhook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/fsthook.zip: \
		$(builddir)/bin/fsthook.so \
		dist/conf/fsthook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/mk3hook.zip: \
		$(builddir)/bin/mk3hook.so \
		dist/piueb \
		dist/conf/mk3hook.conf \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/nx2hook.zip: \
		$(builddir)/bin/nx2hook.so \
		dist/conf/nx2hook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/nxahook.zip: \
		$(builddir)/bin/nxahook.so \
		dist/conf/nxahook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/nxhook.zip: \
		$(builddir)/bin/nxhook.so \
		dist/conf/nxhook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/prihook.zip: \
		$(builddir)/bin/prihook.so \
		dist/conf/prihook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/prohook.zip: \
		$(builddir)/bin/prohook.so \
		dist/conf/prohook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/pro2hook.zip: \
		$(builddir)/bin/pro2hook.so \
		dist/conf/pro2hook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/x2hook.zip: \
		$(builddir)/bin/x2hook.so \
		dist/conf/x2hook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/zerohook.zip: \
		$(builddir)/bin/zerohook.so \
		dist/conf/zerohook.conf \
		dist/piueb \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

################################################
# API: Lexical sorting

# Very messed up and hacky way to preserve the sub-directories as zip does not support this.
# Feel free to improve if you got a more sophisticated solution.
$(zipdir)/api.zip: \
		src/api/ptapi \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)cp -r src/api/ptapi $(builddir)
	$(V)cd $(builddir) && pwd && zip -r api.zip ptapi && cp api.zip zip/api.zip

$(zipdir)/piubtn.zip: \
		$(builddir)/bin/ptapi-io-piubtn-joystick.so \
		$(builddir)/bin/ptapi-io-piubtn-joystick-conf \
		$(builddir)/bin/ptapi-io-piubtn-keyboard.so \
		$(builddir)/bin/ptapi-io-piubtn-keyboard-conf \
		$(builddir)/bin/ptapi-io-piubtn-null.so \
		$(builddir)/bin/ptapi-io-piubtn-real.so \
		$(builddir)/bin/ptapi-io-piubtn-test \
		dist/api/ptapi-io-piubtn-stub.c \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(zipdir)/piuio.zip: \
		$(builddir)/bin/ptapi-io-piuio-joystick.so \
		$(builddir)/bin/ptapi-io-piuio-joystick-conf \
		$(builddir)/bin/ptapi-io-piuio-keyboard.so \
		$(builddir)/bin/ptapi-io-piuio-keyboard-conf \
		$(builddir)/bin/ptapi-io-piuio-null.so \
		$(builddir)/bin/ptapi-io-piuio-real.so \
		$(builddir)/bin/ptapi-io-piuio-test \
		dist/api/ptapi-io-piuio-stub.c \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

################################################
# (USB) profile: Lexical sorting

#		$(builddir)/bin/fex-profile-tool \

$(zipdir)/profile-tools.zip: \
		$(builddir)/bin/nx2-profile-tool \
		$(builddir)/bin/nxa-profile-tool \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

################################################
# pumpnet: Lexical sorting

$(zipdir)/pumpnet.zip: \
		$(builddir)/bin/pumpnet-client \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

################################################
# Security: Lexical sorting

$(zipdir)/security.zip: \
                $(builddir)/bin/microdog40d \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

################################################
# Documentation

$(zipdir)/doc.zip: \
		doc/api/io/piuio.md \
		doc/api/io/x11-input-hook.md \
		doc/game/image \
		doc/game/01-1st.md \
		doc/game/02-2nd.md \
		doc/game/03-3rd.md \
		doc/game/04-3se.md \
		doc/game/05-tc.md \
		doc/game/06-pc.md \
		doc/game/07-extra.md \
		doc/game/08-prem1.md \
		doc/game/09-prex1.md \
		doc/game/10-reb.md \
		doc/game/11-prem2.md \
		doc/game/12-prex2.md \
		doc/game/13-prem3.md \
		doc/game/14-prex3.md \
		doc/game/15-exc.md \
		doc/game/16-exc2.md \
		doc/game/17-zero.md \
		doc/game/18-nx.md \
		doc/game/19-pro.md \
		doc/game/20-nx2.md \
		doc/game/game.md \
		doc/hook/exchook.md \
		doc/hook/hook.md \
		doc/hook/mk3hook.md \
		doc/hook/nxhook.md \
		doc/hook/nx2hook.md \
		doc/hook/os.md \
		doc/hook/prohook.md \
		doc/hook/x2hook.md \
		doc/hook/zerohook.md \
		doc/README.md \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip $@ $^

################################################

$(builddir)/pumptools.zip: \
		$(zipdir)/api.zip \
		$(zipdir)/doc.zip \
		$(zipdir)/exchook.zip \
		$(zipdir)/f2hook.zip \
		$(zipdir)/fexhook.zip \
		$(zipdir)/fsthook.zip \
		$(zipdir)/mk3hook.zip \
		$(zipdir)/nx2hook.zip \
		$(zipdir)/nxahook.zip \
		$(zipdir)/nxhook.zip \
		$(zipdir)/piubtn.zip \
		$(zipdir)/piuio.zip \
		$(zipdir)/profile-tools.zip \
		$(zipdir)/prihook.zip \
		$(zipdir)/pro2hook.zip \
		$(zipdir)/prohook.zip \
		$(zipdir)/pumpnet.zip \
		$(zipdir)/security.zip \
		$(zipdir)/x2hook.zip \
		$(zipdir)/zerohook.zip \
		CHANGELOG.md \
		CONTRIBUTING.md \
		LICENSE \
		README.md \
		$(builddir)/git-version \
		$(builddir)/libc-version
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(builddir)/pumptools-public.zip: \
		$(zipdir)/api.zip \
		$(zipdir)/doc.zip \
		$(zipdir)/exchook.zip \
		$(zipdir)/mk3hook.zip \
		$(zipdir)/nxhook.zip \
		$(zipdir)/nx2hook.zip \
		$(zipdir)/piubtn.zip \
		$(zipdir)/piuio.zip \
		$(zipdir)/prohook.zip \
		$(zipdir)/x2hook.zip \
		$(zipdir)/zerohook.zip \
		CHANGELOG.md \
		LICENSE \
		README.md \
		$(builddir)/git-version \
		$(builddir)/libc-version
	$(V)echo ... $@
	$(V)zip -j $@ $^

################################################

package: $(builddir)/pumptools.zip $(builddir)/pumptools-public.zip
