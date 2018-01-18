/* yt_subs_tracker-window.c
 *
 * Copyright (C) 2017 Mis012
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include <libsoup/soup.h>

#include "yt_subs_tracker-config.h"
#include "yt_subs_tracker-window.h"
#include "main-backend.h"
#include "blank.h"
#include "writeConf.h"

/* 'class' */
struct _Yt_subs_trackerWindow
{
	GtkApplicationWindow  parent_instance;

	/* Template widgets */
	GtkButton        *EditChannelsButton;
	GtkButton        *AboutButton;
	GtkListBox       *SubsList;
	GtkFlowBox       *FlowBox;
	/* add channel dialog */
	GtkDialog        *AddChannelDialog;
	GtkEntry         *FriendlyNameEntry;
	GtkEntry         *XmlFeedEntry;
	/* about dialog */
	GtkDialog        *AboutDialog;

};

G_DEFINE_TYPE (Yt_subs_trackerWindow, yt_subs_tracker_window, GTK_TYPE_APPLICATION_WINDOW)

/* 'some functions (mostly callbacks)' */

gint sortVideos(GtkFlowBoxChild *child1, GtkFlowBoxChild *child2, gpointer data) {
	videoEntry *video1 = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(child1))), "video");
	videoEntry *video2 = g_object_get_data(G_OBJECT(gtk_bin_get_child(GTK_BIN(child2))), "video");
	return strcmp((char *)video2->date, (char *)video1->date);
}

static void playVideo(GtkWidget *event_box, gpointer data) {
	videoEntry *video = g_object_get_data(G_OBJECT(event_box), "video");

	const char *url = "https://www.youtube.com/watch?v=";
	char * full_url = malloc(strlen((char *)video->id) + strlen(url) + 1);
	strcpy(full_url, url);
	strcat(full_url, (char *)video->id);
	printf("selected video: %s\n", full_url);

	pid_t  pid;
	char *argv[] = {"vlc",full_url, NULL};
  	if ((pid = fork()) < 0) {     /* fork a child process           */
			printf("*** ERROR: forking child process failed\n");
				exit(1);
			}
		else if (pid == 0) {          /* for the child process:         */
			if (execvp(*argv, argv) < 0) {     /* execute the command  */
				printf("*** ERROR: exec failed\ndo you have vlc installed?");
				exit(1);
			}
		}
	free(full_url);
}

gboolean onDialogClose(GtkWidget *Dialog, gpointer data) {
	return TRUE;
}

static void showAddChannelDialog(GtkWidget *EditChannelsButton, gpointer AddChannelDialog) {
	gtk_widget_show(AddChannelDialog);
}

void onAddChannelDialogResponse(GtkWidget *AddChannelDialog,gint response_id, gpointer me) {
	Yt_subs_trackerWindow *self = (Yt_subs_trackerWindow *) me;
	if(response_id == GTK_RESPONSE_OK) {
		const char *friendly_name = gtk_entry_get_text(self->FriendlyNameEntry);
		const char *xml_feed_url = gtk_entry_get_text(self->XmlFeedEntry);
		writeConf(friendly_name, xml_feed_url);
	}
	gtk_entry_set_text(self->FriendlyNameEntry, "");
	gtk_widget_hide(AddChannelDialog);
}

static void showAboutDialog(GtkWidget *AboutButton, gpointer AboutDialog) {
	gtk_widget_show(AboutDialog);
}

void onAboutDialogResponse(GtkWidget *AboutDialog,gint response_id, gpointer data) {
	gtk_widget_hide(AboutDialog);
}

void onChannelRemoveClicked (GtkWidget *button, gpointer id) {
	char *channel_id = (char *)id;
	if(removeChannel(channel_id) == 0) { //if we succeeded
	gtk_widget_destroy(gtk_widget_get_ancestor(button, GTK_TYPE_BOX));
	}
}

static void show_image(SoupSession *session, SoupMessage *msg, gpointer thumbnail) {
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new ();
	gdk_pixbuf_loader_write(loader, (guchar *)msg->response_body->data, msg->response_body->length, NULL);
	GdkPixbuf * pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
	gtk_image_set_from_pixbuf (thumbnail, pixbuf);

}

/* the main functions */
static void
yt_subs_tracker_window_class_init (Yt_subs_trackerWindowClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/Yt_Subs_Tracker/yt_subs_tracker-window.ui");
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, EditChannelsButton);
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, AboutButton);
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, SubsList);
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, FlowBox);
	/* add channel dialog */
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, AddChannelDialog);
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, FriendlyNameEntry);
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, XmlFeedEntry);
	/* about dialog */
	gtk_widget_class_bind_template_child (widget_class, Yt_subs_trackerWindow, AboutDialog);

}

static void
yt_subs_tracker_window_init (Yt_subs_trackerWindow *self)
{
	g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

	gtk_widget_init_template (GTK_WIDGET (self));

	gtk_flow_box_set_selection_mode(self->FlowBox, GTK_SELECTION_NONE);
	gtk_flow_box_set_sort_func(self->FlowBox, sortVideos, NULL, NULL);

	g_signal_connect(self->EditChannelsButton, "clicked", (GCallback)showAddChannelDialog, self->AddChannelDialog);
	g_signal_connect_after(self->AddChannelDialog, "delete-event", (GCallback)onDialogClose, NULL);
	g_signal_connect(self->AddChannelDialog, "response", (GCallback)onAddChannelDialogResponse, self);

	g_signal_connect(self->AboutButton, "clicked", (GCallback)showAboutDialog, self->AboutDialog);
	g_signal_connect_after(self->AboutDialog, "delete-event", (GCallback)onDialogClose, NULL);
	g_signal_connect(self->AboutDialog, "response", (GCallback)onAboutDialogResponse, NULL);

	channelArrayStruct * channels_struct = getDataFromBackend();
	if(channels_struct != NULL) {

		SoupSession * session = soup_session_new();

		GError *err = NULL;
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_inline (-1, getBlankThumbnail(), FALSE, &err);
		if(err != NULL) {
					fprintf(stderr, "gdk_pixbuf_new_from_inline failed. error: %s\n", err->message);
					g_error_free (err);
				}

		int i;
		int j;
		for (i = 0; i < channels_struct->count; i++) {
			for (j = 0; j < 15; j++) {
				if(channels_struct->data[i].video_entries[j] == NULL) {
					break;
				}

				GtkWidget *event_box = gtk_event_box_new ();
				GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
				GtkWidget *thumbnail = gtk_image_new_from_pixbuf(pixbuf);
				GtkWidget *label = gtk_label_new((gchar *)channels_struct->data[i].video_entries[j]->title);

				gtk_event_box_set_above_child (GTK_EVENT_BOX(event_box), TRUE);
				g_object_set_data(G_OBJECT(event_box), "video", channels_struct->data[i].video_entries[j]);
				g_signal_connect(event_box, "button_press_event", (GCallback)playVideo, NULL);

				gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
				gtk_label_set_width_chars (GTK_LABEL(label), 50);
				gtk_label_set_max_width_chars (GTK_LABEL(label), 50);

				gtk_container_add(GTK_CONTAINER(box), thumbnail);
				gtk_container_add(GTK_CONTAINER(box), label);


				SoupMessage *msg = soup_message_new ("GET", (char *)channels_struct->data[i].video_entries[j]->thumbnail);
				soup_session_queue_message (session, msg, show_image, thumbnail);

				GtkCssProvider *label_css = gtk_css_provider_new();
				gtk_css_provider_load_from_data(label_css, "* {font-size: 75%;}", -1, NULL);
				gtk_style_context_add_provider (gtk_widget_get_style_context(label), GTK_STYLE_PROVIDER (label_css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

				gtk_widget_show(label);
				gtk_widget_show(thumbnail);
				gtk_widget_show(box);
				gtk_widget_show(event_box);

				gtk_container_add(GTK_CONTAINER (event_box), box);
				gtk_container_add(GTK_CONTAINER (self->FlowBox), event_box);
			}
			GtkWidget *channel_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
			GtkWidget *channel_label = gtk_label_new(channels_struct->data[i].friendly_name);
			gtk_widget_set_hexpand(channel_label, TRUE);
			gtk_widget_set_halign(channel_label, GTK_ALIGN_START);
			GtkWidget *channel_remove_button = gtk_button_new_from_icon_name ("window-close-symbolic", GTK_ICON_SIZE_BUTTON);
			g_signal_connect(channel_remove_button, "clicked", (GCallback)onChannelRemoveClicked, channels_struct->data[i].id);

			gtk_widget_show(channel_label);
			gtk_widget_show(channel_remove_button);
			gtk_widget_show(channel_box);

			gtk_container_add(GTK_CONTAINER(channel_box), channel_label);
			gtk_container_add(GTK_CONTAINER(channel_box), channel_remove_button);
			gtk_container_add(GTK_CONTAINER(self->SubsList), channel_box);
		}
		gtk_flow_box_invalidate_sort(self->FlowBox);
	}



}
