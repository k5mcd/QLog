#!/usr/bin/env bash

# The script finalizes the release and edits the changlog in deployment scripts.
#
# It must be executed from the QLog root directory.

ROOTDIR=.
export QLOG_VERSION=$(head -n 1 ${ROOTDIR}/Changelog  | awk '{print $3}')

echo "Preparing Version ${QLOG_VERSION}"

####### QLOG.pro
echo "Changing QLog.pro"
sed -i "s/VERSION = .*/VERSION = $QLOG_VERSION/" ${ROOTDIR}/QLog.pro

####### Changelog
echo "Changing Changelog"
sed -i "s/TBC - /$(date '+%Y\/%m\/%d') - /" ${ROOTDIR}/Changelog

####### DEB Changelog
echo "Changing DEB Changelog"
echo -e "qlog (${QLOG_VERSION}-1) UNRELEASED; urgency=low\n$(sed -e '/^$/,$d' ${ROOTDIR}/Changelog | tail +2 | sed  "s/^-/  */g")\n\n -- foldynl <foldyna@gmail.com>  $(date "+%a, %-d %b %Y %T %z")\n\n$(cat ${ROOTDIR}/debian/changelog)" > ${ROOTDIR}/debian/changelog

###### RPM Changelog
echo "Changing RPM Changelog"
RPMCHANGERECORDS=$(sed -e '/^$/,$d' ${ROOTDIR}/Changelog | tail +2 | sed "s/^-/-/g")
echo -e "* $(date +"%a %b %-d %Y Ladislav Foldyna - ${QLOG_VERSION}-1")\n$RPMCHANGERECORDS\n" > /tmp/qlog.release.changelog
sed -i -e '/%changelog/{r /tmp/qlog.release.changelog' -e '}' rpm_spec/qlog.spec

##### Appstream Metadata Info Changelog
echo "Changing Appstream Metadata Info Changelog"
XMLCHANGERECORDS=$(sed -e '/^$/,$d' ${ROOTDIR}/Changelog | tail +2 | sed "s/^- //g" | awk -v date="$(date +%Y-%m-%d)" -v version="$QLOG_VERSION" 'BEGIN {print "    <release version=\""version"\" date=\""date"\">\n      <description>\n        <ul>"} {print "          <li>"$0"</li>"} END {print "        </ul>\n      </description>\n    </release>"}')
echo -e "${XMLCHANGERECORDS}" > /tmp/qlog.release.changelog
sed -i -e '/  <releases>/{r /tmp/qlog.release.changelog' -e '}' res/io.github.foldynl.QLog.metainfo.xml
appstreamcli validate ${ROOTDIR}/res/io.github.foldynl.QLog.metainfo.xml

###### QT Deployment files
echo "Changing QT Deployment Files"
sed -i "s/<Version>.*<\/Version>/<Version>${QLOG_VERSION}-1<\/Version>/" ${ROOTDIR}/installer/packages/de.dl2ic.qlog/meta/package.xml
sed -i "s/<ReleaseDate>.*<\/ReleaseDate>/<ReleaseDate>$(date "+%Y-%m-%d")<\/ReleaseDate>/" ${ROOTDIR}/installer/packages/de.dl2ic.qlog/meta/package.xml
sed -i "s/<Version>.*<\/Version>/<Version>${QLOG_VERSION}<\/Version>/" ${ROOTDIR}/installer/config/config.xml

