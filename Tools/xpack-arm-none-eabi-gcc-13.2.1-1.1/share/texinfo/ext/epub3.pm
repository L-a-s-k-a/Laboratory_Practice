# -*-perl-*-

# epub3.pm: setup an EPUB publication
#
#    Copyright 2021 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Originally written by Patrice Dumas.
#
#
# TODO:
#
# Discuss unique identifier on the mailing list
#
# Discuss last change date on the mailing list
#
# Currently the titlepage is used if available, while the Top node
# is not shown.  There is a possibility to use an image as cover in
# EPUB, with cover-image property in a manifest item.  Add the possibility
# to specify such cover image?  In that case, set SHOW_TITLE to 0?
#
# Do not output a TOC in the default case as the readers can always use
# the navigation information?
#
# do something special for indices?
#
# do something special for list of tables/list of floats?
#
# add landmarks?  Examples: epub:type="toc", epub:type="loi" (list of illustrations)
#                           epub:type="bodymatter" (Start of Content)
#
#
# NOTES:
#
# Tests show that the navigation information as a page is not nicely
# rendered, it is better to use the Texinfo TOC if a TOC is needed.
#
# OUTFILE is used for the epub file, but it is reset for the conversion
# to XHTML.  This is described in the documentation.
#
# cross manual references:
# The (X)HTML files that are target of links that point to the EPUB container
# (or maybe any relative link, the standard is not easy to understand) need
# to be in the container (they are non foreign Publication Resources).
# Other <a href= ...> links are not Publication Resources, and are ok.
# Therefore, it is better to specify the external manuals as web HTML
# absolute urls in hyperlinks, and not as manuals in a collection.
# Therefore we set a warning for external manuals not resolved using htmlxref
# by setting  CHECK_HTMLXREF.
#
# Setting up collections of manuals for local browsing:
# It seems that putting more than one manual in an EPUB container
# is incorrect.  Each manual on the command line is therefore put in a
# separate epub container file.
# There is no described way to group manuals in a collection, nor
# to refer to a manual in an epub container, and to a specific file in
# that container.
# Collections of epub manunals for local browsing would be an interesting
# feature, but for now cannot be achieved because of those limitations.
# If references to other EPUB files were possible, NODE_FILES would
# probably need to be set.
# For now, external manuals not found in htmlxref are resolved
# to a path that makes no sense, for example for a reference to the
# Pod-Simple-Texinfo manual:
# EPUB/Pod-Simple-Texinfo_epub3/index.html

use strict;

# for accented character in a comment
use utf8;

# To check if there is no erroneous autovivification
#no autovivification qw(fetch delete exists store strict);

use File::Path;
use File::Spec;
use File::Copy;

# for fileparse
use File::Basename;

use Encode qw(decode);

# also for __(
use Texinfo::Common;
use Texinfo::Convert::Utils;
use Texinfo::Convert::Text;

# try to load here, but only complain and return an error later
# when the customization variables are known.
eval { require Archive::Zip; };

my $archive_zip_loading_error = $@;

# the 3.2 spec was used for the implementation.  However, it seems to be
# designed to be backward compatible with 3.0 and mandates to use 3.0 as
# version.
my $epub_format_version = '3.0';

# used in tests to avoid creating the .epub file.
texinfo_set_from_init_file('EPUB_CREATE_CONTAINER_FILE', 1);

texinfo_set_format_from_init_file('html');

# output valid XHTML as per the specification
# Any Publication Resource that is an XML-Based Media Type MUST
# be a conformant XML 1.0 Document ... MUST be encoded in UTF-8 or UTF-16.
texinfo_set_from_init_file('HTML_ROOT_ELEMENT_ATTRIBUTES',
                           'xmlns="http://www.w3.org/1999/xhtml"');
texinfo_set_from_init_file('NO_CUSTOM_HTML_ATTRIBUTE', 1);
texinfo_set_from_init_file('USE_XML_SYNTAX', 1);
texinfo_set_from_init_file('DOCTYPE', '<?xml version="1.0" encoding="UTF-8"?>'."\n"
                                      .'<!DOCTYPE html>');
texinfo_set_from_init_file('USE_NUMERIC_ENTITY', 1);
texinfo_set_from_init_file('OUTPUT_ENCODING_NAME', 'utf-8');

# this is actually the default
texinfo_set_from_init_file('DOC_ENCODING_FOR_OUTPUT_FILE_NAME', 0);
# the specification says "File Names and Paths MUST be UTF-8 [Unicode] encoded."
# This is also needed for Archive::Zip in case there are non ascii
# file name.
# As a consequence, the epub file file name is also always utf-8 encoded.
texinfo_set_from_init_file('OUTPUT_FILE_NAME_ENCODING', 'utf-8');

# an output as similar as possible to a book is expected for epub.
# So we set NO_TOP_NODE_OUTPUT, which in turn sets titlepage to be used
# if not otherwise specified.
texinfo_set_from_init_file('NO_TOP_NODE_OUTPUT', 1);

# no mini_toc nor menus in the default case, to be more like a book.
texinfo_set_from_init_file('FORMAT_MENU', 'nomenu');

# a footer gets in the way of navigation.  It is not set in the default
# case anyway, but it is set in texi2html style.
texinfo_set_from_init_file('PROGRAM_NAME_IN_FOOTER', 0);

# split at chapter such that ebook readers start a new page for
# a new chapter.  Splitting at nodes output is not so good as node content
# can be very small.
texinfo_set_from_init_file('SPLIT', 'chapter');

# the copiable anchor paragraph sign is always present and no link is
# shown in the calibre epub reader.  Since it looks strange, unset.
texinfo_set_from_init_file('COPIABLE_LINKS', 0);

# this is for the XHTML formatting, the .epub extension is
# also used hardcoded for the container.
texinfo_set_from_init_file('EXTENSION', 'xhtml');

# It is better for external manuals not to be publication resources,
# for that an absolute URL need to be used, so we warn if a manual is not
# found through htmlxref.
texinfo_set_from_init_file('CHECK_HTMLXREF', 1);

# explicit references to TOP_NODE_UP are ignored
texinfo_set_from_init_file('IGNORE_REF_TO_TOP_NODE_UP', 1);

# Better use html for external manuals than the xhtml EXTENSION
texinfo_set_from_init_file('EXTERNAL_CROSSREF_EXTENSION', 'html');

texinfo_set_from_init_file('JS_WEBLABELS_FILE', 'js_licenses.xhtml');

texinfo_set_from_init_file('TOP_FILE', undef);

# no redirections files
texinfo_set_from_init_file('NODE_FILES', 0);

my $epub_images_dir_name = 'images';
texinfo_set_from_init_file('IMAGE_LINK_PREFIX', "../${epub_images_dir_name}/");

#texinfo_set_from_init_file('contents', 1);

texinfo_set_from_init_file('DEFAULT_RULE', '');
texinfo_set_from_init_file('BIG_RULE', '');
texinfo_set_from_init_file('HEADERS', 0);

texinfo_register_formatting_function('format_navigation_header', \&epub_noop);
texinfo_register_formatting_function('format_navigation_panel', \&epub_noop);

texinfo_register_command_formatting('image', \&epub_convert_image_command);

texinfo_register_type_formatting('unit', \&epub_convert_tree_unit_type);

my %epub_images_extensions_mimetypes = (
  '.png' =>  'image/png',
  '.jpg' => 'image/jpeg',
  '.jpeg' => 'image/jpeg',
  '.gif' => 'image/gif',
);

my %epub_js_extensions_mimetypes = (
  '.js', 'text/javascript',
  '.css', 'text/css',
);

sub _epub_convert_tree_to_text($$;$)
{
  my $converter = shift;
  my $tree = shift;
  my $options = shift;

  $options = {} if (!defined($options));

  return &{$converter->formatting_function('format_protect_text')}($converter,
    Texinfo::Convert::Text::convert_to_text($tree,
   {Texinfo::Convert::Text::copy_options_for_convert_text($converter, 1),
     %$options}));
}

sub epub_noop($$)
{
  return '';
}

# file scope variables. Beware to reset all that are not constants
# at the beginning of epub_setup for multi input Texinfo manual cases.
my $epub_destination_directory;
my $epub_document_destination_directory;
my $encoded_epub_destination_directory;

my $epub_document_dir_name = 'EPUB';

my %epub_images;

# the image number, to make sure that there is no name clash when
# putting all the images in the same directory.
my $epub_file_nr;

# collect and copy images
sub epub_convert_image_command($$$$)
{
  my $self = shift;
  my $cmdname = shift;
  my $command = shift;
  my $args = shift;

  if (defined($args->[0]->{'monospacetext'})
      and $args->[0]->{'monospacetext'} ne '') {
    my $basefile = $args->[0]->{'monospacetext'};
    return $basefile if ($self->in_string());

    my ($image_file, $image_basefile, $image_extension, $image_path,
        $image_path_encoding)
      = $self->html_image_file_location_name($cmdname, $command, $args);
    if (not defined($image_path)) {
      # FIXME using an internal function.  Also not clear if it is correct to
      # use it, as it is not used for other messages
      $self->_noticed_line_warn(sprintf(
              __("\@image file `%s' (for HTML) not found, using `%s'"),
                 $image_basefile, $image_file), $command->{'source_info'});
    }

    my ($volume, $directories, $image_basefile_name)
      = File::Spec->splitpath($image_basefile);
    my $protected_image_basefile_name
      = Texinfo::Convert::NodeNameNormalization::transliterate_protect_file_name(
                                                         $image_basefile_name);
    my $protected_image_extension
      = Texinfo::Convert::NodeNameNormalization::transliterate_protect_file_name(
                                                         $image_extension);
    # -5 for the extension and -10 for $epub_file_nr
    my $cropped_image_basefile_name
     = substr($protected_image_basefile_name, 0,
              $self->get_conf('BASEFILENAME_LENGTH') - 15);
    my $destination_basefile_name = $epub_file_nr.'-'.$cropped_image_basefile_name
                                    . $protected_image_extension;
    $epub_file_nr += 1;
    if (defined($image_file)) {
      if (not defined($image_path)) {
        $self->document_error($self,
              sprintf(__("\@image file `%s' can not be copied"),
                     $image_basefile));
      } else {
        my $images_destination_dir
               = File::Spec->catdir($epub_destination_directory,
                                    $epub_document_dir_name, $epub_images_dir_name);
        my ($encoded_images_destination_dir, $images_destination_dir_encoding)
          = $self->encoded_output_file_name($images_destination_dir);
        my $error_creating_dir;
        if (! -d $encoded_images_destination_dir) {
          if (!mkdir($encoded_images_destination_dir, oct(755))) {
            $self->document_error($self, sprintf(__(
                             "could not create images directory `%s': %s"),
                                         $images_destination_dir, $!));
            $error_creating_dir = 1;
          }
        }
        if (not $error_creating_dir) {
          my $image_destination_path_name
             = File::Spec->catfile($images_destination_dir,
                                   $destination_basefile_name);
          my ($encoded_image_dest_path_name, $image_dest_path_encoding)
            = $self->encoded_output_file_name($image_destination_path_name);
          my $copy_succeeded = copy($image_path, $encoded_image_dest_path_name);
          if (not $copy_succeeded) {
            my $image_path_text;
            if (defined($image_path_encoding)) {
              $image_path_text = decode($image_path_encoding, $image_path);
            } else {
              $image_path_text = $image_path;
            }
            $self->document_error($self, sprintf(__(
                     "could not copy `%s' to `%s': %s"),
                        $image_path_text, $image_destination_path_name, $!));
          }
          $epub_images{$destination_basefile_name} = $image_extension;
        }
      }
    }
    # Now format.  Following code is similar to the default formatting
    # code.
    my $destination_file_name;
    # should always be set
    if (defined($self->get_conf('IMAGE_LINK_PREFIX'))) {
      $destination_file_name = $self->get_conf('IMAGE_LINK_PREFIX')
                                   . $destination_basefile_name;
    } else {
      $destination_file_name = $destination_basefile_name;
    }
    my $alt_string;
    if (defined($args->[3]) and defined($args->[3]->{'string'})) {
      $alt_string = $args->[3]->{'string'};
    }
    if (!defined($alt_string) or ($alt_string eq '')) {
      $alt_string
       = &{$self->formatting_function('format_protect_text')}($self, $basefile);
    }

    return $self->close_html_lone_element(
      $self->html_attribute_class('img', [$cmdname])
        . ' src="'.$self->url_protect_file_text($destination_file_name)
           ."\" alt=\"$alt_string\"");
  }
  return '';
}

my @epub_output_filenames;
# collect filenames in units order
sub epub_convert_tree_unit_type($$$$)
{
  my $self = shift;
  my $type = shift;
  my $element = shift;
  my $content = shift;

  push @epub_output_filenames, $element->{'structure'}->{'unit_filename'}
    unless grep {$_ eq $element->{'structure'}->{'unit_filename'}}
            @epub_output_filenames;
  return &{$self->default_type_conversion($type)}($self,
                                      $type, $element, $content);
}

sub _epub_remove_container_folder($$)
{
  my $self = shift;
  my $encoded_epub_destination_directory = shift;
  my $err_remove_tree;
  File::Path::remove_tree($encoded_epub_destination_directory,
                          {'error' => $err_remove_tree});
  if ($err_remove_tree and scalar(@$err_remove_tree)) {
    for my $diag (@$err_remove_tree) {
      my ($file, $message) = %$diag;
      if ($file eq '') {
        $self->document_error($self,
           sprintf(__("error removing directory: %s: %s"),
                   $epub_destination_directory, $message));
      }
      else {
        $self->document_error($self,
          sprintf(__("error removing directory: %s: unlinking %s: %s"),
                  $epub_destination_directory, $file, $message));
      }
    }
    return 1;
  }
  return 0;
}

my $epub_xhtml_dir = 'xhtml';
# should not clash with generated files.  Could clash with
# OUTFILE but this case is explicitly handled below.
my $default_nav_filename = 'nav_toc.xhtml';
my $nav_filename;

my $epub_outfile;

my $epub_info_js_dir_name;

sub epub_setup($)
{
  my $self = shift;

  $epub_outfile = undef;
  $epub_destination_directory = undef;
  $epub_document_destination_directory = undef;
  $encoded_epub_destination_directory = undef;
  @epub_output_filenames = ();
  %epub_images = ();
  $nav_filename = $default_nav_filename;
  $epub_file_nr = 1;

  if (not defined($self->get_conf('EPUB_CREATE_CONTAINER_FILE'))) {
    if (not $self->get_conf('TEST')) {
      $self->set_conf('EPUB_CREATE_CONTAINER_FILE', 1);
    }
  }

  if ($self->get_conf('EPUB_CREATE_CONTAINER_FILE')
      and $archive_zip_loading_error) {
    $self->document_error($self,
       __("Archive::Zip is required for EPUB file output"));
    return 150;
  }

  if (not defined($self->get_conf('EPUB_KEEP_CONTAINER_FOLDER'))) {
    if ($self->get_conf('TEST') or $self->get_conf('DEBUG')) {
      $self->set_conf('EPUB_KEEP_CONTAINER_FOLDER', 1);
    }
  }
  
  $epub_info_js_dir_name = undef;
  if ($self->get_conf('INFO_JS_DIR')) {
    # re-set INFO_JS_DIR up to have the javascript and
    # css files in a directory rooted at $epub_document_dir_name
    $epub_info_js_dir_name = $self->get_conf('INFO_JS_DIR');
    my $updir = File::Spec->updir();
    # FIXME INFO_JS_DIR is used both as a filesystem directory name
    # and as path in HTML files.  As a path in HTML, / should always be
    # used.  File::Spec->catdir is better for filesystem paths.  We finally
    # hardocde '/' as separator because it is needed for HTML paths and it
    # works for both for Unix and Windows filesystems, but it is not
    # clean.
    #$self->force_conf('INFO_JS_DIR', File::Spec->catdir($updir,
    #                                              $epub_info_js_dir_name));
    $self->force_conf('INFO_JS_DIR', join('/', ($updir,
                                                  $epub_info_js_dir_name)));
    # TODO make sure it is SPLIT and set SPLIT if not?
  }

  # determine main epub directory and directory for xhtml files,
  # reset OUTFILE and SUBDIR to match with the epub directory
  # for XHTML output
  
  if (defined($self->get_conf('OUTFILE'))) {
    $epub_outfile = $self->get_conf('OUTFILE');
    # if not undef, will be used as directory name in
    # determine_files_and_directory() which does not make sense
    if ($self->get_conf('SPLIT')) {
      $self->force_conf('OUTFILE', undef);
    }
  }
  my ($output_file, $destination_directory, $output_filename,
      $document_name) = $self->determine_files_and_directory('epub_package');
  if (not defined($epub_outfile)) {
    $epub_outfile = ${document_name}.'.epub';
  }
  # the $epub_destination_directory is removed automatically,
  # so we try to set it to a directory that the user would not create
  # nor populate with files.
  if (defined($self->get_conf('SUBDIR'))) {
    $epub_destination_directory = File::Spec->catdir($self->get_conf('SUBDIR'),
                                          $document_name . '_epub_package');
  } elsif ($self->get_conf('SPLIT')) {
    $epub_destination_directory = $destination_directory;
  } else {
    $epub_destination_directory = $document_name . '_epub_package';
  }
  $epub_document_destination_directory
             = File::Spec->catdir($epub_destination_directory,
                                  $epub_document_dir_name, $epub_xhtml_dir);
  # set for XHTML conversion
  if ($self->get_conf('SPLIT')) {
    $self->force_conf('SUBDIR', $epub_document_destination_directory);
    $self->force_conf('OUTFILE', undef);
  } else {
    my $xhtml_output_file = $document_name;
    $xhtml_output_file .= '.'.$self->get_conf('EXTENSION')
        if (defined($self->get_conf('EXTENSION'))
            and $self->get_conf('EXTENSION') ne '');
    # to avoid a clash with nav file name
    if ($xhtml_output_file eq $default_nav_filename) {
      $nav_filename = 'Gtexinfo_' . $default_nav_filename;
    }
    $self->force_conf('OUTFILE',
     File::Spec->catfile($epub_document_destination_directory, $xhtml_output_file));
  }

  my $epub_destination_dir_encoding;
  ($encoded_epub_destination_directory, $epub_destination_dir_encoding)
    = $self->encoded_output_file_name($epub_destination_directory);

  my $status = _epub_remove_container_folder($self,
                                       $encoded_epub_destination_directory);
  return $status if ($status);

  my $err_make_path;
  my ($encoded_epub_document_destination_directory, $epub_doc_dest_dir_encoding)
    = $self->encoded_output_file_name($epub_document_destination_directory);
  File::Path::make_path($encoded_epub_document_destination_directory,
                        {'mode' => 0755, 'error' => $err_make_path});
  if ($err_make_path and scalar(@$err_make_path)) {
    for my $diag (@$err_make_path) {
      my ($file, $message) = %$diag;
      if ($file eq '') {
        $self->document_error($self,
           sprintf(__("error creating directory: %s: %s"),
                  $epub_document_destination_directory, $message));
      }
      else {
        $self->document_error($self,
          sprintf(__("error creating directory: %s: creating %s: %s"),
                 $epub_document_destination_directory, $file, $message));
      }
    }
    return 150;
  }
  return 0;
}

texinfo_register_handler('setup', \&epub_setup);

# need to be after tree units and images conversion
sub epub_finish($$)
{
  my $self = shift;
  my $document_root = shift;

  if (scalar(@epub_output_filenames) == 0) {
    if (defined($self->{'current_filename'})) {
      push @epub_output_filenames, $self->{'current_filename'};
    } else {
      $self->document_warn($self,
        __("epub: no filename output"));
    }
  }

  my $meta_inf_directory_name = 'META-INF';
  my $meta_inf_directory = File::Spec->catdir($epub_destination_directory,
                                              $meta_inf_directory_name);
  my ($encoded_meta_inf_directory, $meta_inf_directory_encoding)
    = $self->encoded_output_file_name($meta_inf_directory);
  if (!mkdir($encoded_meta_inf_directory, oct(755))) {
    $self->document_error($self, sprintf(__(
                   "could not create meta informations directory `%s': %s"),
                                         $meta_inf_directory, $!));
    return 1;
  }
  my $container_file_path_name = File::Spec->catfile($meta_inf_directory,
                                           'container.xml');
  my ($encoded_container_file_path_name, $container_path_encoding)
    = $self->encoded_output_file_name($container_file_path_name);
  my ($container_fh, $error_message_container)
            = Texinfo::Common::output_files_open_out(
                          $self->output_files_information(), $self,
                          $encoded_container_file_path_name, undef, 'utf-8');
  if (!defined($container_fh)) {
    $self->document_error($self,
         sprintf(__("epub3.pm: could not open %s for writing: %s\n"),
                  $container_file_path_name, $error_message_container));
    return 1;
  }
  my $document_name = $self->get_info('document_name');
  my $opf_filename = $document_name . '.opf';
  print $container_fh <<EOT;
<?xml version="1.0"?>
<container version="1.0" xmlns="urn:oasis:names:tc:opendocument:xmlns:container">
    <rootfiles>
        <rootfile full-path="${epub_document_dir_name}/${opf_filename}"
            media-type="application/oebps-package+xml" />	
    </rootfiles>
</container>
EOT

  Texinfo::Common::output_files_register_closed(
    $self->output_files_information(), $encoded_container_file_path_name);
  if (!close ($container_fh)) {
    $self->document_error($self,
         sprintf(__("epub3.pm: error on closing %s: %s"),
                          $container_file_path_name, $!));
    return 1;
  }

  my $mimetype_filename = 'mimetype';
  my $mimetype_file_path_name = File::Spec->catfile($epub_destination_directory,
                                                    $mimetype_filename);
  my ($encoded_mimetype_file_path_name, $mimetype_path_encoding)
    = $self->encoded_output_file_name($mimetype_file_path_name);
  my ($mimetype_fh, $error_message_mimetype)
               = Texinfo::Common::output_files_open_out(
                        $self->output_files_information(), $self,
                        $encoded_mimetype_file_path_name, undef, 'utf-8');
  if (!defined($mimetype_fh)) {
    $self->document_error($self,
         sprintf(__("epub3.pm: could not open %s for writing: %s\n"),
                  $mimetype_file_path_name, $error_message_mimetype));
    return 1;
  }
  # There is no end of line.  It is not very clear in the standard, but
  # example files demonstrate clearly that there should not be end of lines.
  print $mimetype_fh 'application/epub+zip';

  Texinfo::Common::output_files_register_closed(
    $self->output_files_information(), $encoded_mimetype_file_path_name);
  if (!close ($mimetype_fh)) {
    $self->document_error($self,
         sprintf(__("epub3.pm: error on closing %s: %s"),
                          $mimetype_file_path_name, $!));
    return 1;
  }
  my $nav_id = 'nav';
  my $nav_file_path_name;
  my $title = _epub_convert_tree_to_text($self, $self->get_info('title_tree'));
  if ($self->{'structuring'} and $self->{'structuring'}->{'sectioning_root'}) {
    $nav_file_path_name
     = File::Spec->catfile($epub_document_destination_directory, $nav_filename);
    my ($encoded_nav_file_path_name, $nav_path_encoding)
      = $self->encoded_output_file_name($nav_file_path_name);
    my ($nav_fh, $error_message_nav)
           = Texinfo::Common::output_files_open_out(
                       $self->output_files_information(), $self,
                       $encoded_nav_file_path_name, undef, 'utf-8');
    if (!defined($nav_fh)) {
      $self->document_error($self,
           sprintf(__("epub3.pm: could not open %s for writing: %s\n"),
                    $nav_file_path_name, $error_message_nav));
      return 1;
    }
    my $table_of_content_str = _epub_convert_tree_to_text($self,
                                             $self->gdt('Table of contents'));
    my $nav_file_title = $title.' - '.$table_of_content_str;
    print $nav_fh <<EOT;
<?xml version="1.0" encoding="utf-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops">
<head>
<meta charset="utf-8" />
<title>$nav_file_title</title>
</head>
<body>
<nav epub:type="toc" id="$nav_id">
<h1>$table_of_content_str</h1>
EOT

    # similar code as in chm.pm
    my $section_root = $self->{'structuring'}->{'sectioning_root'};
    my $upper_level = $section_root->{'structure'}->{'section_childs'}->[0]
                                            ->{'structure'}->{'section_level'};
    foreach my $top_section (@{$section_root->{'structure'}->{'section_childs'}}) {
      $upper_level = $top_section->{'structure'}->{'section_level'}
      if ($top_section->{'structure'}->{'section_level'} < $upper_level);
    }
    $upper_level = 1 if ($upper_level <= 0);
    my $root_level = $upper_level - 1;
    my $level = $root_level;
    foreach my $section (@{$self->{'structuring'}->{'sections_list'}}) {
      next if ($section->{'cmdname'} eq 'part');
      my $section_level = $section->{'structure'}->{'section_level'};
      $section_level = 1 if ($section_level == 0);
      # FIXME with gaps in sectioning there could be nesting issues?
      if ($level < $section_level) {
        while ($level < $section_level) {
          my $leading_spaces = '';
          print $nav_fh "\n". " " x $level . "<ol>\n";
          $level++;
        }
      } elsif ($level > $section->{'structure'}->{'section_level'}) {
        # on the same line as the a element for the first </li>
        print $nav_fh "</li>\n". " " x ($level -1) . "</ol>\n";
        $level--;
        while ($level > $section_level) {
          print $nav_fh " " x $level . "</li>\n". " " x ($level -1) . "</ol>\n";
          $level--;
        }
        print $nav_fh " " x $level ."</li>\n";
      } else {
        print $nav_fh "</li>\n";
      }
      my $text = _epub_convert_tree_to_text($self, $section->{'args'}->[0]);
      $text
        = Texinfo::Convert::Utils::add_heading_number($self, $section, $text,
                                          $self->get_conf('NUMBER_SECTIONS'));
      # the empty string as second argument makes sure that the
      # source file is different from the target file.
      my $origin_href = $self->command_href($section, '');
      print $nav_fh " " x $level . "<li><a href=\"$origin_href\">$text</a>";
    }
    if ($level > $root_level) {
      # on the same line as the a element for the first </li>
      print $nav_fh "</li>\n". " " x ($level -1) . "</ol>\n";
      $level--;
    }
    while ($level > $root_level) {
      print $nav_fh " " x $level . "</li>\n". " " x ($level -1) . "</ol>\n";
      $level--;
    }

    print $nav_fh '</nav>'."\n";
    # TODO add landmarks?
    print $nav_fh '</body>'."\n".'</html>'."\n";

    Texinfo::Common::output_files_register_closed(
      $self->output_files_information(), $encoded_nav_file_path_name);
    if (!close ($nav_fh)) {
      $self->document_error($self,
           sprintf(__("epub3.pm: error on closing %s: %s"),
                            $nav_file_path_name, $!));
      return 1;
    }
  }


  my $unique_uid = 'texi-uid';
  # TODO to discuss on bug-texinfo
  my $identifier = 'texinfo:'.$document_name;
  # FIXME the dcterms:modified is mandatory, and it is also mandatory that it is a date:
  #  each Rendition MUST include exactly one [DCTERMS] modified property containing its last modification date. The value of this property MUST be an [XMLSCHEMA-2] dateTime conformant date of the form:

  # CCYY-MM-DDThh:mm:ssZ
  #
  # The last modification date MUST be expressed in Coordinated Universal Time (UTC) and MUST be terminated by the "Z" (Zulu) time zone indicator.
  #
  # <meta property="dcterms:modified">2012-03-05T12:47:00Z</meta>
  # to discuss
  # <dc:rights>
  my $opf_file_path_name = File::Spec->catfile($epub_destination_directory,
                                        $epub_document_dir_name, $opf_filename);
  my ($encoded_opf_file_path_name, $opf_path_encoding)
    = $self->encoded_output_file_name($opf_file_path_name);
  my ($opf_fh, $error_message_opf)
            = Texinfo::Common::output_files_open_out(
                   $self->output_files_information(), $self,
                   $encoded_opf_file_path_name, undef, 'utf-8');
  if (!defined($opf_fh)) {
    $self->document_error($self,
         sprintf(__("epub3.pm: could not open %s for writing: %s\n"),
                  $opf_file_path_name, $error_message_opf));
    return 1;
  }
  print $opf_fh <<EOT;
<?xml version="1.0" encoding="UTF-8"?>
<package xmlns="http://www.idpf.org/2007/opf" version="$epub_format_version" unique-identifier="$unique_uid">
   <metadata xmlns:dc="http://purl.org/dc/elements/1.1/">
      <dc:identifier id="$unique_uid">$identifier</dc:identifier>
      <dc:title>$title</dc:title>
EOT
  my @relevant_commands = ('author', 'documentlanguage');
  my $collected_commands = Texinfo::Common::collect_commands_list_in_tree(
                                        $document_root, \@relevant_commands);
  my @authors = ();
  my @languages = ();
  if (scalar(@{$collected_commands})) {
    foreach my $element (@{$collected_commands}) {
      my $command = $element->{'cmdname'};
      if ($command eq 'author') {
        if ($element->{'extra'}->{'titlepage'}
             and $element->{'args'}->[0]->{'contents'}) {
          my $author_str = _epub_convert_tree_to_text($self,
               {'contents' => $element->{'args'}->[0]->{'contents'}});
          if ($author_str =~ /\S/) {
            push @authors, $author_str;
          }
        }
      } else {
        if (defined($element->{'extra'}->{'text_arg'})) {
          # TODO the EPUB specification describes specific language
          # tags.  Not sure there is not a need for some mapping here.
          push @languages, $element->{'extra'}->{'text_arg'};
        }
      }
    }
  }
  # the standard mandates at least one language specifier
  if (scalar(@languages) == 0) {
    @languages = ('en');
  }
  foreach my $language (@languages) {
    print $opf_fh "      <dc:language>$language</dc:language>\n";
  }
  foreach my $author (@authors) {
    print $opf_fh "      <dc:creator>$author</dc:creator>\n";
  }
  print $opf_fh <<EOT;
   </metadata>
   <manifest>
EOT

  if (defined($nav_file_path_name)) {
    print $opf_fh "      <item id=\"$nav_id\" properties=\"nav\" "
      . "media-type=\"application/xhtml+xml\" href=\"${epub_xhtml_dir}/${nav_filename}\"/>\n";
  }
  my $spine_uid_str = 'unit';
  my @output_filename_ids = ();
  my $id_count = 0;
  foreach my $output_filename (@epub_output_filenames) {
    $id_count++;
    my $properties_str = '';
    if ($self->get_conf('INFO_JS_DIR')) {
      $properties_str = ' properties="scripted"'
    }
    print $opf_fh "      <item id=\"${spine_uid_str}${id_count}\" "
     . "media-type=\"application/xhtml+xml\" href=\"${epub_xhtml_dir}/${output_filename}\"${properties_str}/>\n";
  }
  my $js_weblabels_id;
  if ($self->get_conf('JS_WEBLABELS_FILE')) {
    my $js_weblabels_file_name = $self->get_conf('JS_WEBLABELS_FILE');
    my $js_licenses_file_path = File::Spec->catfile($epub_document_destination_directory,
                                                    $js_weblabels_file_name);
    if (-e $js_licenses_file_path) {
      $js_weblabels_id = 'jsweblabels';
      print $opf_fh "      <item id=\"${js_weblabels_id}\" "
     . "media-type=\"application/xhtml+xml\" href=\"${epub_xhtml_dir}/${js_weblabels_file_name}\"/>\n";
    }
  }
  my $image_count = 0;
  foreach my $image_file (sort keys(%epub_images)) {
    $image_count++;
    my $image_extension = $epub_images{$image_file};
    my $image_mimetype;
    if (defined($epub_images_extensions_mimetypes{$image_extension})) {
      $image_mimetype = $epub_images_extensions_mimetypes{$image_extension};
    } else {
      my $extension = $image_extension;
      $extension =~ s/^\.//;
      $image_mimetype = $extension . '/image';
    }
    print $opf_fh "      <item id=\"image${image_count}\" "
      . "media-type=\"${image_mimetype}\" href=\"${epub_images_dir_name}/${image_file}\"/>\n";
  }
  if (defined($epub_info_js_dir_name)) {
    my $info_js_destination_dir
               = File::Spec->catdir($epub_destination_directory,
                                    $epub_document_dir_name, $epub_info_js_dir_name);
    my $opendir_success = opendir(JSPATH, $info_js_destination_dir);
    if (not $opendir_success) {
      $self->document_error($self,
           sprintf(__("epub3.pm: readdir %s error: %s"),
                          $info_js_destination_dir, $!));
    } else {
      my $js_count = 0;
      foreach my $filename (sort(readdir(JSPATH))) {
        my ($parsed_filename, $parsed_directory, $suffix)
             = fileparse($filename, keys(%epub_js_extensions_mimetypes));
        if (defined($suffix) and $suffix ne '') {
          $js_count++;
          my $js_mimetype = $epub_js_extensions_mimetypes{$suffix};
          print $opf_fh "      <item id=\"infojs${js_count}\" "
  . "media-type=\"${js_mimetype}\" href=\"${epub_info_js_dir_name}/${filename}\"/>\n";
        }
      }
      closedir(JSPATH);
    }
  }
  print $opf_fh <<EOT;
   </manifest>
   <spine>
EOT

  $id_count = 0;
  foreach my $output_filename (@epub_output_filenames) {
    $id_count++;
    print $opf_fh "      <itemref idref=\"${spine_uid_str}${id_count}\"/>\n";
  }
  # Depending on the reader, the js_labels file should better be in the <spine> or
  # not.  The standard allows both showing the linear="no" elements as part
  # of the default reading order or not.  It is probably better for the
  # js_labels to be in the spine if they can be viewed in any way.
  # Foliate does not show the js_labels file upon clicking if not in
  # the <spine>.
  # Calibre shows the js_labels file upon clicking if not in the <spine>, and
  # steps in the js_labels file if in the spine.
  if (defined($js_weblabels_id)) {
    print $opf_fh "      <itemref idref=\"${js_weblabels_id}\" linear=\"no\"/>\n";
  }

  print $opf_fh <<EOT;
   </spine>
</package>
EOT

  Texinfo::Common::output_files_register_closed(
    $self->output_files_information(), $encoded_opf_file_path_name);
  if (!close ($opf_fh)) {
    $self->document_error($self,
         sprintf(__("epub3.pm: error on closing %s: %s"),
                          $opf_file_path_name, $!));
    return 1;
  }

  if ($self->get_conf('EPUB_CREATE_CONTAINER_FILE')) {
    # this is needed if there are non ascii file names, otherwise, for instance
    # with calibre the files cannot be read, one get
    # "There is no item named 'EPUB/osé.opf' in the archive"
    # even though unzip -l lists the file well.  More testing is probably
    # needed on other plaforms.
    local $Archive::Zip::UNICODE = 1;
    my $zip = Archive::Zip->new();
    # the standard says that the mimetype file should not be compressed
    # The mimetype file MUST NOT be compressed or encrypted
    my $mimetype_added
      = $zip->addFile($encoded_mimetype_file_path_name, $mimetype_filename,
                      Archive::Zip->COMPRESSION_LEVEL_NONE);
    if (not(defined($mimetype_added))) {
      $self->document_error($self,
        sprintf(__("epub3.pm: error adding %s to archive"),
               $mimetype_file_path_name));
      return 1;
    }

    my $meta_inf_directory_ret_code
      = $zip->addTree($encoded_meta_inf_directory, $meta_inf_directory_name);
    if ($meta_inf_directory_ret_code != Archive::Zip->AZ_OK) {
      $self->document_error($self,
        sprintf(__("epub3.pm: error adding %s to archive"),
               $meta_inf_directory));
      return 1;
    }

    my $epub_document_dir_path = File::Spec->catdir($epub_destination_directory,
                                                    $epub_document_dir_name);
    my ($encoded_epub_document_dir_path, $epub_document_dir_path_encoding)
      = $self->encoded_output_file_name($epub_document_dir_path);
    my $epub_document_dir_name_ret_code
      = $zip->addTree($encoded_epub_document_dir_path, $epub_document_dir_name);
    if ($epub_document_dir_name_ret_code != Archive::Zip->AZ_OK) {
      $self->document_error($self,
        sprintf(__("epub3.pm: error adding %s to archive"),
               $epub_document_dir_path));
      return 1;
    }

    my ($encoded_epub_outfile, $epub_outfile_encoding)
      = $self->encoded_output_file_name($epub_outfile);
    unless ($zip->writeToFileNamed($encoded_epub_outfile) == Archive::Zip->AZ_OK) {
      $self->document_error($self,
           sprintf(__("epub3.pm: error writing archive %s"),
                   $epub_outfile));
      return 1;
    }
  }

  if (not $self->get_conf('EPUB_KEEP_CONTAINER_FOLDER')) {
    my $status = _epub_remove_container_folder($self,
                                       $encoded_epub_destination_directory);
    return $status if ($status);
  }
  return 0;
}

texinfo_register_handler('finish', \&epub_finish);

1;
