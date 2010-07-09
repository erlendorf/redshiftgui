#include "common.h"
#include "iup.h"
#include "options.h"
#include "gamma.h"
#include "iupgui_main.h"
#include "iupgui_gamma.h"

// Settings dialog handles
static Ihandle *dialog_settings=NULL;
static Ihandle *label_day=NULL;
static Ihandle *label_night=NULL;
static Ihandle *label_transpeed=NULL;
static Ihandle *val_night=NULL;
static Ihandle *val_day=NULL;
static Ihandle *val_transpeed=NULL;

// Settings - temp day changed
static int _val_day_changed(Ihandle *ih){
	int val = IupGetInt(ih,"VALUE");
	int rounded = 100*((int)(val/100.0f));
	IupSetfAttribute(label_day,"TITLE","%d� K",rounded);
	gamma_state_set_temperature(opt_get_method(),rounded,opt_get_gamma());
	return IUP_DEFAULT;
}

// Settings - temp night changed
static int _val_night_changed(Ihandle *ih){
	int val = IupGetInt(ih,"VALUE");
	int rounded = 100*((int)(val/100.0f));
	IupSetfAttribute(label_night,"TITLE","%d� K",rounded);
	gamma_state_set_temperature(opt_get_method(),rounded,opt_get_gamma());
	return IUP_DEFAULT;
}

// Settings - transition speed changed
static int _val_speed_changed(Ihandle *ih){
	int val = IupGetInt(ih,"VALUE");
	IupSetfAttribute(label_transpeed,"TITLE","%d� K/s",val);
	return IUP_DEFAULT;
}

// Setting - cancel
static int _setting_cancel(Ihandle *ih){
	guigamma_check(ih);
	IupDestroy(dialog_settings);
	dialog_settings = NULL;
	return IUP_DEFAULT;
}

// Setting - save
static int _setting_save(Ihandle *ih){
	int vday = 100*((int)(IupGetInt(val_day,"VALUE")/100.0f));
	int vnight = 100*((int)(IupGetInt(val_night,"VALUE")/100.0f));

	LOG(LOGVERBOSE,_("New day temp: %d, new night temp: %d"),vday,vnight);
	opt_set_temperatures(vday,vnight);
	opt_set_transpeed(IupGetInt(val_transpeed,"VALUE"));
	opt_write_config();
	guigamma_check(ih);
	IupDestroy(dialog_settings);
	dialog_settings = NULL;
	return IUP_DEFAULT;
}

// Creates settings dialog
static void _settings_create(void){
	Ihandle *listmethod,
			*vbox_method,
			*frame_method,

			*vbox_day,
			*frame_day,

			*vbox_night,
			*frame_night,

			*vbox_transpeed,
			*frame_speed,

			*button_cancel,
			*button_save,
			*hbox_buttons,
			*vbox_all;
	extern Ihandle *himg_redshift;

	// Method selection
	listmethod = IupList(NULL);
	IupSetAttribute(listmethod,"DROPDOWN","YES");
	IupSetAttribute(listmethod,"EXPAND","YES");
	vbox_method = IupVbox(
			listmethod,
			NULL);
	frame_method = IupFrame(vbox_method);
	IupSetAttribute(frame_method,"TITLE","Backend");

	// Day temperature
	label_day = IupLabel(NULL);
	IupSetfAttribute(label_day,"TITLE",_("%d� K"),opt_get_temp_day());
	val_day = IupVal(NULL);
	IupSetAttribute(val_day,"EXPAND","YES");
	IupSetfAttribute(val_day,"MIN","%d",MIN_TEMP);
	IupSetfAttribute(val_day,"MAX","%d",MAX_TEMP);
	IupSetfAttribute(val_day,"VALUE","%d",opt_get_temp_day());
	IupSetCallback(val_day,"VALUECHANGED_CB",(Icallback)_val_day_changed);
	vbox_day = IupVbox(
			label_day,
			val_day,
			NULL);
	IupSetAttribute(vbox_day,"MARGIN","5");
	frame_day = IupFrame(vbox_day);
	IupSetAttribute(frame_day,"TITLE",_("Day Temperature"));

	// Night temperature
	label_night = IupLabel(NULL);
	IupSetfAttribute(label_night,"TITLE",_("%d� K"),opt_get_temp_night());
	val_night = IupVal(NULL);
	IupSetAttribute(val_night,"EXPAND","YES");
	IupSetfAttribute(val_night,"MIN","%d",MIN_TEMP);
	IupSetfAttribute(val_night,"MAX","%d",MAX_TEMP);
	IupSetfAttribute(val_night,"VALUE","%d",opt_get_temp_night());
	IupSetCallback(val_night,"VALUECHANGED_CB",(Icallback)_val_night_changed);
	vbox_night = IupVbox(
			label_night,
			val_night,
			NULL);
	IupSetAttribute(vbox_night,"MARGIN","5");
	frame_night = IupFrame(vbox_night);
	IupSetAttribute(frame_night,"TITLE","Night Temperature");

	// Transition speed
	label_transpeed = IupLabel(NULL);
	IupSetfAttribute(label_transpeed,"TITLE",_("%d� K/s"),opt_get_trans_speed());
	val_transpeed = IupVal(NULL);
	IupSetAttribute(val_transpeed,"EXPAND","YES");
	IupSetfAttribute(val_transpeed,"MIN","%d",MIN_SPEED);
	IupSetfAttribute(val_transpeed,"MAX","%d",MAX_SPEED);
	IupSetfAttribute(val_transpeed,"VALUE","%d",opt_get_trans_speed());
	IupSetCallback(val_transpeed,"VALUECHANGED_CB",(Icallback)_val_speed_changed);
	vbox_transpeed = IupVbox(
			label_transpeed,
			val_transpeed,
			NULL);
	IupSetAttribute(vbox_transpeed,"MARGIN","5");
	frame_speed = IupFrame(vbox_transpeed);
	IupSetAttribute(frame_speed,"TITLE","Transition Speed");

	// Buttons
	button_cancel = IupButton(_("Cancel"),NULL);
	IupSetfAttribute(button_cancel,"MINSIZE","%dx%d",60,24);
	IupSetCallback(button_cancel,"ACTION",(Icallback)_setting_cancel);
	button_save = IupButton(_("Save"),NULL);
	IupSetfAttribute(button_save,"MINSIZE","%dx%d",60,24);
	IupSetCallback(button_save,"ACTION",(Icallback)_setting_save);
	hbox_buttons = IupHbox(
			button_cancel,
			button_save,
			NULL);

	// Box containing all
	vbox_all = IupVbox(
			frame_method,
			frame_day,
			frame_night,
			frame_speed,
			hbox_buttons,
			NULL);
	IupSetfAttribute(vbox_all,"NMARGIN","%dx%d",5,5);
	IupSetAttribute(vbox_all,"ALIGNMENT","ARIGHT");

	dialog_settings=IupDialog(vbox_all);
	IupSetAttribute(dialog_settings,"TITLE",_("Settings"));
	IupSetfAttribute(dialog_settings,"RASTERSIZE","%dx%d",200,300);
	IupSetAttributeHandle(dialog_settings,"ICON",himg_redshift);
}

// Shows the settings dialog
int guisettings_show(Ihandle *ih){
	if( !dialog_settings )
		_settings_create();
	IupPopup(dialog_settings,IUP_CENTER,IUP_CENTER);
	if( guimain_get_exit() )
		return IUP_CLOSE;
	return IUP_DEFAULT;
}

// Closes settings dialog
int guisettings_close(Ihandle *ih){
	if( dialog_settings ){
		IupDestroy( dialog_settings );
		dialog_settings = NULL;
	}
	return IUP_DEFAULT;
}