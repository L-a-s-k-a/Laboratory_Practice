# TexinfoXML.pm: output tree as Texinfo XML.
#
# Copyright 2011, 2012, 2013, 2016, 2017, 2018 Free Software Foundation, Inc.
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
# Original author: Patrice Dumas <pertusus@free.fr>
#
# A simple subclass of the Texinfo::Convert::TexinfoMarkup abstract
# class.  Defines format specific functions.

package Texinfo::Convert::TexinfoXML;

use 5.00405;
use strict;

use Carp qw(cluck);

use Texinfo::Convert::TexinfoMarkup;

# for xml formatting methods
use Texinfo::Convert::Converter;

use vars qw($VERSION @ISA);
@ISA = qw(Texinfo::Convert::TexinfoMarkup Texinfo::Convert::Converter);

$VERSION = '7.0.3';


# TexinfoXML specific
my %defaults = (
  'ENABLE_ENCODING'      => 0,
  'FORMAT_MENU'          => 'menu',
  'EXTENSION'            => 'xml',
  'OUTPUT_ENCODING_NAME' => 'utf-8',
  'converted_format'     => 'xml',
  'SPLIT'                => 0,
  'documentlanguage'     => 'en',
);

sub converter_defaults($$)
{
  return %defaults;
}


# Following are XML specific formatting functions.

sub txi_markup_protect_text($$)
{
  my $self = shift;
  my $string = shift;
  return $self->_protect_text($string);
}

my %special_xml_attributes = (
  'documentlanguage' => {'lang' => 'xml:lang'},
  # for preformatted type
  'pre' => {'space' => 'xml:space'},
  'verbatim' => {'space' => 'xml:space'},
);

sub _xml_attributes($$)
{
  my $self = shift;
  my $attributes = shift;
  my $format_element = shift;
  if (ref($attributes) ne 'ARRAY') {
    cluck "attributes not an array($attributes).";
  }
  my $result = '';
  foreach my $attribute_spec (@$attributes) {
    if (ref($attribute_spec) ne 'ARRAY') {
       cluck "attribute_spec not an array($attribute_spec).";
    }
    # this cannot be used because of formfeed, as '<', which
    # is substituted from &formfeed; is not allowed in attribute.
    #my $text = $self->_protect_text($attribute_spec->[1]);
    my $text = $self->xml_protect_text($attribute_spec->[1]);
    # in fact form feed is not allowed at all in XML, even protected
    # and even in xml 1.1 in contrast to what is said on internet.
    # maybe this is a limitation of libxml?
    #$text =~ s/\f/&#12;/g;
    if ($attribute_spec->[0] ne 'spaces'
        and $attribute_spec->[0] ne 'trailingspaces') {
      $text =~ s/\f/&attrformfeed;/g;
      # &attrformfeed; resolves to \f so \ are doubled
      $text =~ s/\\/\\\\/g;
    }
    my $attribute_name = $attribute_spec->[0];
    if ($special_xml_attributes{$format_element}
        and $special_xml_attributes{$format_element}->{$attribute_spec->[0]}) {
      $attribute_name
        = $special_xml_attributes{$format_element}->{$attribute_spec->[0]};
    }
    $result .= " $attribute_name=\"".$text."\"";
  }
  return $result;
}

sub txi_markup_element($$$)
{
  my $self = shift;
  my $format_element = shift;
  my $attributes = shift;
  my $result= '<'.$format_element;
  $result .= $self->_xml_attributes($attributes, $format_element)
        if ($attributes);
  $result .= '/>';
  return $result;
}

sub txi_markup_open_element($$$)
{
  my $self = shift;
  my $format_element = shift;
  my $attributes = shift;
  my $result= '<'."$format_element";
  $result .= $self->_xml_attributes($attributes, $format_element)
      if ($attributes);
  $result .= '>';
  return $result;
}

sub txi_markup_close_element($$)
{
  my $self = shift;
  my $format_element = shift;
  my $result= "</$format_element>";
  return $result;
}

my %no_arg_commands_formatting
   = %Texinfo::Convert::TexinfoMarkup::no_arg_commands_formatting;

sub txi_markup_atom($$)
{
  my $self = shift;
  my $atom = shift;
  if ($no_arg_commands_formatting{$atom} ne '') {
    return '&'.$no_arg_commands_formatting{$atom}.';';
  } else {
    return '';
  }
}

sub txi_markup_comment($$)
{
  my $self = shift;
  my $string = shift;

  return $self->xml_comment($string);
}

# form feed is not accepted in xml, replace it.
sub _protect_text($$)
{
  my $self = shift;
  my $text = shift;
  my $result = $self->xml_protect_text($text);
  $result =~ s/\f/&formfeed;/g;
  return $result;
}

sub txi_markup_convert_text($$)
{
  my $self = shift;
  my $element = shift;
  my $result = $self->_protect_text($element->{'text'});
  if (! defined($element->{'type'}) or $element->{'type'} ne 'raw') {
    # FIXME API
    if (!$self->in_monospace()) {
      $result =~ s/``/&textldquo;/g;
      $result =~ s/\'\'/&textrdquo;/g;
      $result =~ s/---/&textmdash;/g;
      $result =~ s/--/&textndash;/g;
      $result =~ s/'/&textrsquo;/g;
      $result =~ s/`/&textlsquo;/g;
    }
  }
  return $result;
}

sub txi_markup_header($)
{
  my $self = shift;

  my $encoding = '';
  if ($self->get_conf('OUTPUT_ENCODING_NAME')
      and $self->get_conf('OUTPUT_ENCODING_NAME') ne 'utf-8') {
    $encoding = " encoding=\"".$self->get_conf('OUTPUT_ENCODING_NAME')."\" ";
  }
  my $texinfo_dtd_version = $self->get_conf('TEXINFO_DTD_VERSION');
  if (!defined($texinfo_dtd_version)) {
    $texinfo_dtd_version = '1.00';
  }

  # In the TexinfoML name, the X of XML is replaced by Texinfo.
  my $header =  "<?xml version=\"1.0\"${encoding}?>".'
<!DOCTYPE texinfo PUBLIC "-//GNU//DTD TexinfoML V'.$texinfo_dtd_version.'//EN" "http://www.gnu.org/software/texinfo/dtd/'.$texinfo_dtd_version.'/texinfo.dtd">
';
  return $header;
}

1;

__END__
# Automatically generated from maintain/template.pod

=head1 NAME

Texinfo::Convert::TexinfoXML - Convert Texinfo tree to TexinfoXML

=head1 SYNOPSIS

  my $converter
    = Texinfo::Convert::TexinfoXML->converter({'parser' => $parser});

  $converter->output($tree);
  $converter->convert($tree);
  $converter->convert_tree($tree);

=head1 NOTES

The Texinfo Perl module main purpose is to be used in C<texi2any> to convert
Texinfo to other formats.  There is no promise of API stability.

=head1 DESCRIPTION

Texinfo::Convert::TexinfoXML converts a Texinfo tree to TexinfoXML.

=head1 METHODS

=over

=item $converter = Texinfo::Convert::TexinfoXML->converter($options)

Initialize converter from Texinfo to TexinfoXML.

The I<$options> hash reference holds options for the converter.  In
this option hash reference a L<parser object|Texinfo::Parser>
may be associated with the I<parser> key.  The other options
are Texinfo customization options and a few other options that can
be passed to the converter. Most of the customization options are described in
the Texinfo manual.  Those customization options, when appropriate, override
the document content.  The parser should not be available directly anymore
after getting the associated information.

See L<Texinfo::Convert::Converter> for more information.

=item $converter->output($tree)

Convert a Texinfo tree I<$tree> and output the result in files as
described in the Texinfo manual.

=item $result = $converter->convert($tree)

Convert a Texinfo tree I<$tree> and return the resulting output.

=item $result = $converter->convert_tree($tree)

Convert a Texinfo tree portion I<$tree> and return the resulting
output.  This function does not try to output a full document but only
portions.  For a full document use C<convert>.

=back

=head1 AUTHOR

Patrice Dumas, E<lt>pertusus@free.frE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2010- Free Software Foundation, Inc.  See the source file for
all copyright years.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

=cut
