#include "dbobject.h"

#include "core.h"
#include "fds.h"

#include "Alitheia.h"

#include "CORBA.h"

#include <sstream>
#include <fstream>

using namespace Alitheia;

using namespace eu::sqooss::impl::service::corba;

using std::string;
using std::istream;
using std::stringbuf;
using std::ostream;
using std::ofstream;
using std::cout;
using std::endl;

DAObject::DAObject( int id )
    : id( id )
{
}

DAObject::~DAObject()
{
}

StoredProject::StoredProject( const alitheia::StoredProject& project )
    : DAObject( project.id ),
      name( project.name ),
      website( project.website ),
      contact( project.contact ),
      bugs( project.bugs ),
      repository( project.repository ),
      mail( project.mail )
{
}

alitheia::StoredProject StoredProject::toCorba() const
{
    alitheia::StoredProject result;
    result.id = id;
    result.name = CORBA::string_dup( name.c_str() );
    result.website = CORBA::string_dup( website.c_str() );
    result.contact = CORBA::string_dup( contact.c_str() );
    result.bugs = CORBA::string_dup( bugs.c_str() );
    result.repository = CORBA::string_dup( repository.c_str() );
    return result;
}

StoredProject::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

StoredProject StoredProject::fromCorba( const CORBA::Any& any )
{
    alitheia::StoredProject project;
    any >>= project;
    return StoredProject( project );
}

ProjectVersion::ProjectVersion( const alitheia::ProjectVersion& version )
    : DAObject( version.id ),
      project( version.project ),
      version( version.version ),
      timeStamp( version.timeStamp ),
      committer( version.committer ),
      commitMsg( version.commitMsg ),
      properties( version.properties )
{
}

alitheia::ProjectVersion ProjectVersion::toCorba() const
{
    alitheia::ProjectVersion result;
    result.id = id;
    result.project = project.toCorba();
    result.version = version;
    result.timeStamp = timeStamp;
    result.committer = committer.toCorba();
    result.commitMsg = CORBA::string_dup( commitMsg.c_str() );
    result.properties = CORBA::string_dup( properties.c_str() );
    return result;
}

ProjectVersion::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

ProjectVersion ProjectVersion::fromCorba( const CORBA::Any& any )
{
    alitheia::ProjectVersion version;
    any >>= version;
    return ProjectVersion( version );
}

std::vector< ProjectFile > ProjectVersion::getVersionFiles() const
{
    return Core::instance()->getVersionFiles( *this );
}

namespace Alitheia
{
class ProjectFileBuffer : public stringbuf
{
public:
    ProjectFileBuffer( const ProjectFile* file )
        : file( file ),
          read( false )
    {
    }

protected:
    int underflow()
    {
        if( !read )
        {
            FDS fds;
            string data = fds.getFileContents( *file );
            sputn( data.c_str(), data.size() );
            read = true;
        }
        return stringbuf::underflow();
    }

private:
    const ProjectFile* const file;
    bool read;
};
}

ProjectFile::ProjectFile()
    : istream( new ProjectFileBuffer( this ) ),
      DAObject( 0 ),
      isDirectory( false )
{
}

ProjectFile::ProjectFile( const alitheia::ProjectFile& file )
    : istream( new ProjectFileBuffer( this ) ),
      DAObject( file.id ),
      name( file.name ),
      projectVersion( file.version ),
      status( file.status ),
      isDirectory( file.isDirectory ),
      directory( file.dir )
{
}

ProjectFile::ProjectFile( const ProjectFile& other )
    : istream( new ProjectFileBuffer( this ) )
{
    *this = other;
}

ProjectFile& ProjectFile::operator=( const ProjectFile& other )
{
    id = other.id;
    name = other.name;
    projectVersion = other.projectVersion;
    status = other.status;
    isDirectory = other.isDirectory;
    directory = other.directory;
    return *this;
}

void ProjectFile::save( ostream& stream ) const
{
    // copy intented
    ProjectFile projectFile = *this;
    string line;
    do
    {
        std::getline( projectFile, line );
        if( !projectFile.eof() )
            line.push_back( '\n' );
        stream.write( line.c_str(), line.size() );
    } while( !projectFile.eof() );
}

void ProjectFile::save( const string& filename ) const
{
    ofstream file( filename.c_str() );
    save( file );
}

alitheia::ProjectFile ProjectFile::toCorba() const
{
    alitheia::ProjectFile result;
    result.id = id;
    result.name = CORBA::string_dup( name.c_str() );
    result.version = projectVersion.toCorba();
    result.status = CORBA::string_dup( status.c_str() );
    result.isDirectory = isDirectory;
    result.dir = directory.toCorba();
    return result;
}

ProjectFile::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

ProjectFile ProjectFile::fromCorba( const CORBA::Any& any )
{
    alitheia::ProjectFile file;
    any >>= file;
    return ProjectFile( file );
}

ProjectFile::~ProjectFile()
{
    delete rdbuf();
}

FileGroup::FileGroup( const alitheia::FileGroup& group )
    : DAObject( group.id ),
      name( group.name ),
      subPath( group.subPath ),
      regex( group.regex ),
      recalcFreq( group.recalcFreq ),
      lastUsed( group.lastUsed ),
      projectVersion( group.version )
{
}

alitheia::FileGroup FileGroup::toCorba() const
{
    alitheia::FileGroup result;
    result.id = id;
    result.name = CORBA::string_dup( name.c_str() );
    result.subPath = CORBA::string_dup( subPath.c_str() );
    result.regex = CORBA::string_dup( regex.c_str() );
    result.recalcFreq = recalcFreq;
    result.lastUsed = CORBA::string_dup( lastUsed.c_str() );
    result.version = projectVersion.toCorba();
    return result;
}

FileGroup::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

FileGroup FileGroup::fromCorba( const CORBA::Any& any )
{
    alitheia::FileGroup group;
    any >>= group;
    return FileGroup( group );
}

MetricType::MetricType( const alitheia::MetricType& type )
    : DAObject( type.id ),
      type( static_cast< Type >( type.type ) )
{
}

alitheia::MetricType MetricType::toCorba() const
{
    alitheia::MetricType result;
    result.id = id;
    result.type = static_cast< alitheia::MetricTypeType >( type );
    return result;
}

MetricType::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

MetricType MetricType::fromCorba( const CORBA::Any& any )
{
    alitheia::MetricType type;
    any >>= type;
    return MetricType( type );
}

Plugin::Plugin( const alitheia::Plugin& plugin )
    : DAObject( plugin.id ),
      name( plugin.name ),
      installdate( plugin.installdate )
{
}

alitheia::Plugin Plugin::toCorba() const
{
    alitheia::Plugin result;
    result.id = id;
    result.name = CORBA::string_dup( name.c_str() );
    result.installdate = CORBA::string_dup( installdate.c_str() );
    return result;
}

Plugin::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

Plugin Plugin::fromCorba( const CORBA::Any& any )
{
    alitheia::Plugin plugin;
    any >>= plugin;
    return Plugin( plugin );
}

Metric::Metric( const alitheia::Metric& metric )
    : DAObject( metric.id ),
      plugin( metric.metricPlugin ),
      metricType( metric.type ),
      mnemonic( metric.mnemonic ),
      description( metric.description )
{
}

alitheia::Metric Metric::toCorba() const
{
    alitheia::Metric result;
    result.id = id;
    result.metricPlugin = plugin.toCorba();
    result.type = metricType.toCorba();
    result.mnemonic = CORBA::string_dup( mnemonic.c_str() );
    result.description = CORBA::string_dup( description.c_str() );
    return result;
}

Metric::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

Metric Metric::fromCorba( const CORBA::Any& any )
{
    alitheia::Metric metric;
    any >>= metric;
    return Metric( metric );
}

ProjectFileMeasurement::ProjectFileMeasurement( const alitheia::ProjectFileMeasurement& measurement )
    : DAObject( measurement.id ),
      metric( measurement.measureMetric ),
      projectFile( measurement.file ),
      whenRun( measurement.whenRun ),
      result( measurement.result )
{
}

alitheia::ProjectFileMeasurement ProjectFileMeasurement::toCorba() const
{
    alitheia::ProjectFileMeasurement result;
    result.id = id;
    result.measureMetric = metric.toCorba();
    result.file = projectFile.toCorba();
    result.whenRun = CORBA::string_dup( whenRun.c_str() );
    result.result = CORBA::string_dup( this->result.c_str() );
    return result;
}

ProjectFileMeasurement::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

ProjectFileMeasurement ProjectFileMeasurement::fromCorba( const CORBA::Any& any )
{
    alitheia::ProjectFileMeasurement measurement;
    any >>= measurement;
    return ProjectFileMeasurement( measurement );
}

ProjectVersionMeasurement::ProjectVersionMeasurement( const alitheia::ProjectVersionMeasurement& measurement )
    : DAObject( measurement.id ),
      metric( measurement.measureMetric ),
      projectVersion( measurement.version ),
      whenRun( measurement.whenRun ),
      result( measurement.result )
{
}

alitheia::ProjectVersionMeasurement ProjectVersionMeasurement::toCorba() const
{
    alitheia::ProjectVersionMeasurement result;
    result.id = id;
    result.measureMetric = metric.toCorba();
    result.version = projectVersion.toCorba();
    result.whenRun = CORBA::string_dup( whenRun.c_str() );
    result.result = CORBA::string_dup( this->result.c_str() );
    return result;
}

ProjectVersionMeasurement::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

ProjectVersionMeasurement ProjectVersionMeasurement::fromCorba( const CORBA::Any& any )
{
    alitheia::ProjectVersionMeasurement measurement;
    any >>= measurement;
    return ProjectVersionMeasurement( measurement );
}

Developer::Developer( const alitheia::Developer& developer )
    : DAObject( developer.id ),
      name( developer.name ),
      email( developer.email ),
      username( developer.username ),
      storedProject( developer.project )
{
}

alitheia::Developer Developer::toCorba() const
{
    alitheia::Developer result;
    result.id = id;
    result.name = CORBA::string_dup( name.c_str() );
    result.email = CORBA::string_dup( email.c_str() );
    result.username = CORBA::string_dup( username.c_str() );
    result.project = storedProject.toCorba();
    return result;
}

Developer::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

Developer Developer::fromCorba( const CORBA::Any& any )
{
    alitheia::Developer dev;
    any >>= dev;
    return Developer( dev );
}

Directory::Directory( const alitheia::Directory& directory )
    : DAObject( directory.id ),
      path( directory.path )
{
}

alitheia::Directory Directory::toCorba() const
{
    alitheia::Directory result;
    result.id = id;
    result.path = CORBA::string_dup( path.c_str() );
    return result;
}

Directory::operator CORBA::Any() const
{
    CORBA::Any any;
    any <<= toCorba();
    return any;
}

Directory Directory::fromCorba( const CORBA::Any& any )
{
    alitheia::Directory dir;
    any >>= dir;
    return Directory( dir );
}
