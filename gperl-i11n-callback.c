/* -*- mode: c; indent-tabs-mode: t; c-basic-offset: 8; -*- */

static GPerlI11nPerlCallbackInfo *
create_perl_callback_closure (GITypeInfo *cb_type, SV *code)
{
	GPerlI11nPerlCallbackInfo *info;

	info = g_new0 (GPerlI11nPerlCallbackInfo, 1);
	if (!gperl_sv_is_defined (code))
		return info;

	info->interface =
		(GICallableInfo *) g_type_info_get_interface (cb_type);
	info->cif = g_new0 (ffi_cif, 1);
	info->closure =
		g_callable_info_prepare_closure (info->interface, info->cif,
		                                 invoke_callback, info);
	/* FIXME: This should most likely use SvREFCNT_inc instead of
	 * newSVsv. */
	info->code = newSVsv (code);
	info->sub_name = NULL;

#ifdef PERL_IMPLICIT_CONTEXT
	info->priv = aTHX;
#endif

	return info;
}

static void
attach_perl_callback_data (GPerlI11nPerlCallbackInfo *info, SV *data)
{
	/* FIXME: SvREFCNT_inc? */
	info->data = newSVsv (data);
}

/* assumes ownership of sub_name */
static GPerlI11nPerlCallbackInfo *
create_perl_callback_closure_for_named_sub (GITypeInfo *cb_type, gchar *sub_name)
{
	GPerlI11nPerlCallbackInfo *info;

	info = g_new0 (GPerlI11nPerlCallbackInfo, 1);
	info->interface =
		(GICallableInfo *) g_type_info_get_interface (cb_type);
	info->cif = g_new0 (ffi_cif, 1);
	info->closure =
		g_callable_info_prepare_closure (info->interface, info->cif,
		                                 invoke_callback, info);
	info->sub_name = sub_name;
	info->code = NULL;
	info->data = NULL;

#ifdef PERL_IMPLICIT_CONTEXT
	info->priv = aTHX;
#endif

	return info;
}

static void
release_perl_callback (gpointer data)
{
	GPerlI11nPerlCallbackInfo *info = data;
	dwarn ("releasing Perl callback info %p\n", info);

	/* g_callable_info_free_closure reaches into info->cif, so it needs to
	 * be called before we free it.  See
	 * <https://bugzilla.gnome.org/show_bug.cgi?id=652954>. */
	if (info->closure)
		g_callable_info_free_closure (info->interface, info->closure);
	if (info->cif)
		g_free (info->cif);

	if (info->interface)
		g_base_info_unref ((GIBaseInfo*) info->interface);

	if (info->code)
		SvREFCNT_dec (info->code);
	if (info->data)
		SvREFCNT_dec (info->data);
	if (info->sub_name)
		g_free (info->sub_name);

	g_free (info);
}

/* -------------------------------------------------------------------------- */

static GPerlI11nCCallbackInfo *
create_c_callback_closure (GIBaseInfo *interface, gpointer func)
{
	GPerlI11nCCallbackInfo *info;

	info = g_new0 (GPerlI11nCCallbackInfo, 1);
	if (!func)
		return info;

	info->interface = interface;
	g_base_info_ref (interface);
	info->func = func;

	return info;
}

static void
attach_c_callback_data (GPerlI11nCCallbackInfo *info, gpointer data)
{
	info->data = data;
}

static void
release_c_callback (gpointer data)
{
	GPerlI11nCCallbackInfo *info = data;
	dwarn ("releasing C callback info %p\n", info);

	/* FIXME: we cannot call the destroy notify here because it might be
	 * our own release_perl_callback which would try to free the ffi stuff
	 * that is currently running. */
	/* if (info->destroy) */
	/* 	info->destroy (info->data); */

	if (info->interface)
		g_base_info_unref (info->interface);

	g_free (info);
}
