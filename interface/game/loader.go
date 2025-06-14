package game

import (
	"fmt"
	"io/ioutil"
	"path/filepath"
	"strings"
)

const (
	// GamesDirectory diretório padrão dos jogos
	GamesDirectory = "Jogos"
	// ROMExtension extensão dos arquivos ROM
	ROMExtension = ".nes"
)

// GameInfo contém informações sobre um jogo
type GameInfo struct {
	Name     string // Nome para exibição
	FileName string // Nome do arquivo
	FullPath string // Caminho completo
}

// Loader responsável pelo carregamento de ROMs
type Loader struct {
	gamesDir string
}

// NewLoader cria um novo carregador de jogos
func NewLoader() *Loader {
	return &Loader{
		gamesDir: GamesDirectory,
	}
}

// NewLoaderWithDir cria um novo carregador com diretório customizado
func NewLoaderWithDir(dir string) *Loader {
	return &Loader{
		gamesDir: dir,
	}
}

// LoadGameList carrega a lista de jogos disponíveis
func (l *Loader) LoadGameList() ([]GameInfo, error) {
	files, err := ioutil.ReadDir(l.gamesDir)
	if err != nil {
		return nil, fmt.Errorf("erro ao ler diretório %s: %w", l.gamesDir, err)
	}

	var games []GameInfo
	for _, file := range files {
		if file.IsDir() {
			continue
		}

		if !strings.HasSuffix(strings.ToLower(file.Name()), ROMExtension) {
			continue
		}

		gameInfo := GameInfo{
			Name:     l.formatGameName(file.Name()),
			FileName: file.Name(),
			FullPath: filepath.Join(l.gamesDir, file.Name()),
		}
		games = append(games, gameInfo)
	}

	return games, nil
}

// formatGameName formata o nome do arquivo para exibição
func (l *Loader) formatGameName(fileName string) string {
	// Remove a extensão
	name := strings.TrimSuffix(fileName, ROMExtension)
	// Substitui underscores por espaços
	name = strings.ReplaceAll(name, "_", " ")
	return name
}

// LoadROM carrega uma ROM específica (placeholder para implementação futura)
func (l *Loader) LoadROM(gamePath string) error {
	// TODO: Implementar carregamento real da ROM
	// Por enquanto, apenas verifica se o arquivo existe
	if _, err := ioutil.ReadFile(gamePath); err != nil {
		return fmt.Errorf("erro ao carregar ROM %s: %w", gamePath, err)
	}
	return nil
}

// GetDefaultErrorGames retorna jogos de erro padrão quando não há ROMs
func GetDefaultErrorGames() []GameInfo {
	return []GameInfo{
		{
			Name:     "Nenhum jogo .nes encontrado",
			FileName: "",
			FullPath: "",
		},
	}
}