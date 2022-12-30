import subprocess
import re
import argparse
from lxml import etree
from datetime import datetime

def generate_appcast(sparkle_path, signing_key_file, dmg_file, src_appcast):
    # Generate signature
    result_pattern = r'sparkle:edSignature="(?P<signature>[a-zA-Z0-9=+]{88})" length="(?P<length>[\d]+)"'
    p = subprocess.Popen([sparkle_path+"/bin/sign_update", "-f", signing_key_file, dmg_file], 
        stdout=subprocess.PIPE, 
        stderr=subprocess.PIPE)
    out, err = p.communicate()
    # Parse sign_update output into usable variables
    m = re.match(result_pattern, out.decode("utf-8").strip())
    print(m.group('signature'))
    print(m.group('length'))

    # Get publish date
    now = datetime.now()
    pub_date = now.strftime("%a, %d %m %Y %H:%M:%S")

    # Parse source appcast.xml
    tree = etree.parse(src_appcast)
    rss_root = tree.getroot()
    sparkle_namespace = rss_root.nsmap['sparkle']
    xsparkle = "{%s}" % sparkle_namespace
    channel = next(rss_root.iter("channel"))
    # Create new update item
    xitem = etree.SubElement(channel, "item")
    xitem_title = etree.SubElement(xitem, "title")
    xitem_title.text = "1.2.3"
    xitem_pubdate = etree.SubElement(xitem, "pubDate")
    xitem_pubdate.text = pub_date
    xitem_link = etree.SubElement(xitem, "link")
    xitem_link.text = "https://swiftray.io/"
    xitem_sparkle_version = etree.SubElement(xitem, xsparkle + "version")
    xitem_sparkle_version.text = "1.2.3"
    xitem_sparkle_short_version = etree.SubElement(xitem, xsparkle + "shortVersionString")
    xitem_sparkle_short_version.text = "1.2"
    xitem_sparkle_min_system_ver = etree.SubElement(xitem, xsparkle + "minimumSystemVersion")
    xitem_sparkle_min_system_ver.text = "10.13"
    xitem_sparkle_release_link = etree.SubElement(xitem, xsparkle + "releaseNotesLink")
    xitem_sparkle_release_link.text = "https://swiftray_notes.aws.s3/swiftray_1.2.3.html"
    xitem_enclosure = etree.SubElement(xitem, "enclosure")
    xitem_enclosure.set('url', "https://swiftray.aws.s3/swiftray_1.1.3.dmg")
    xitem_enclosure.set(xsparkle + "edSignature", m.group('signature'))
    xitem_enclosure.set('length', m.group('length'))
    xitem_enclosure.set('type', "application/octet-stream")
    #etree.tostring(xitem, pretty_print=True)
    #etree.tostring(rss_root, pretty_print=True)

    return etree.tostring(rss_root, pretty_print=True)


def main():
    parser = argparse.ArgumentParser(description='Sparkle appcast generator')
    parser.add_argument('--sparkle', dest='sparkle_path', type=str, required=True,
                        help='Path to the sparkle')
    parser.add_argument('--signkey', dest='signing_key_file', type=str, required=True,
                        help='Path to the signing key')
    parser.add_argument('--dmg', dest='dmg_file', type=str, required=True,
                        help='Path to the dmg file')
    parser.add_argument('--src_appcast', dest='src_appcast', type=str, required=True,
                        help='Path to the source appcast file')
    options = parser.parse_args()

    xml_str = generate_appcast(options.sparkle_path, 
                      options.signing_key_file, 
                      options.dmg_file, 
                      options.src_appcast)
    # TODO: Write to file
    print(xml_str)

if __name__ == "__main__":
    main()